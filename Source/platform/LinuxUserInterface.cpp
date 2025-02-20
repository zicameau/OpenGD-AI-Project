#include "LinuxUserInterface.h"
#include <gtk/gtk.h>
#include <libnotify/notify.h>
#include <iostream>
#include <stdexcept>
#include <syslog.h>

LinuxUserInterface* LinuxUserInterface::getInstance() {
    static LinuxUserInterface instance;
    return &instance;
}

void LinuxUserInterface::showError(const std::string& message, const std::string& details) {
    GtkWidget *dialog = gtk_message_dialog_new(
        nullptr,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", message.c_str()
    );
    
    gtk_message_dialog_format_secondary_text(
        GTK_MESSAGE_DIALOG(dialog),
        "%s", details.c_str()
    );
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (_errorCallback) {
        _errorCallback(message + ": " + details);
    }
}

bool LinuxUserInterface::showConfigDialog() {
    std::lock_guard<std::mutex> lock(_configMutex);
    try {
        // Create dialog with RAII
        std::unique_ptr<GtkWidget, void(*)(GtkWidget*)> dialog(
            gtk_dialog_new_with_buttons(
                "Configuration",
                nullptr,
                GTK_DIALOG_MODAL,
                "_OK",
                GTK_RESPONSE_ACCEPT,
                "_Cancel",
                GTK_RESPONSE_CANCEL,
                nullptr
            ),
            [](GtkWidget* w) { if (w) gtk_widget_destroy(w); }
        );

        if (!dialog) {
            throw std::runtime_error("Failed to create configuration dialog");
        }

        // Run dialog and return result
        gint result = gtk_dialog_run(GTK_DIALOG(dialog.get()));
        return result == GTK_RESPONSE_ACCEPT;

    } catch (const std::exception& e) {
        syslog(LOG_ERR, "Configuration dialog error: %s", e.what());
        return false;
    }
}

bool LinuxUserInterface::initializeSystemTray() {
    if (!_config.showSystemTray) return false;
    
    notify_init("OpenGD");
    
    GtkStatusIcon *trayIcon = gtk_status_icon_new_from_file("icon.png");
    gtk_status_icon_set_visible(trayIcon, TRUE);
    
    GtkMenu *menu = GTK_MENU(gtk_menu_new());
    
    // Add menu items
    for (const auto& item : _trayMenuItems) {
        GtkWidget *menuItem = gtk_menu_item_new_with_label(item.first.c_str());
        g_signal_connect(menuItem, "activate", G_CALLBACK(+[](GtkMenuItem*, gpointer data) {
            auto callback = reinterpret_cast<std::function<void()>*>(data);
            (*callback)();
        }), (gpointer)&item.second);
        gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    }
    
    gtk_widget_show_all(GTK_WIDGET(menu));
    
    g_signal_connect(trayIcon, "popup-menu", G_CALLBACK(+[](GtkStatusIcon*, guint button, 
                                                           guint activate_time, gpointer menu) {
        gtk_menu_popup(GTK_MENU(menu), nullptr, nullptr, nullptr, nullptr, button, activate_time);
    }), menu);
    
    return true;
}

void LinuxUserInterface::updateTrayIcon(const std::string& icon) {
    // Update tray icon implementation
}

void LinuxUserInterface::addTrayMenuItem(const std::string& label, std::function<void()> callback) {
    _trayMenuItems.emplace_back(label, callback);
}

void LinuxUserInterface::showUpdateNotification() {
    if (!_config.enableNotifications) return;
    
    NotifyNotification *notification = notify_notification_new(
        "OpenGD Update Available",
        "A new version of OpenGD is available. Click here to update.",
        "system-software-update"
    );
    
    notify_notification_set_urgency(notification, NOTIFY_URGENCY_NORMAL);
    notify_notification_show(notification, nullptr);
    g_object_unref(G_OBJECT(notification));
}

bool LinuxUserInterface::showUpdateDialog() {
    GtkWidget *dialog = gtk_message_dialog_new(
        nullptr,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_YES_NO,
        "A new version of OpenGD is available. Would you like to update now?"
    );
    
    bool result = (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_YES);
    gtk_widget_destroy(dialog);
    return result;
}

void LinuxUserInterface::showUpdateProgress(float progress) {
    static GtkWidget *progressDialog = nullptr;
    static GtkWidget *progressBar = nullptr;
    
    if (!progressDialog) {
        progressDialog = gtk_dialog_new_with_buttons(
            "Updating OpenGD",
            nullptr,
            GTK_DIALOG_MODAL,
            "_Cancel", GTK_RESPONSE_CANCEL,
            nullptr
        );
        
        progressBar = gtk_progress_bar_new();
        gtk_container_add(
            GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(progressDialog))),
            progressBar
        );
        gtk_widget_show_all(progressDialog);
    }
    
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), progress);
    
    if (progress >= 1.0f) {
        gtk_widget_destroy(progressDialog);
        progressDialog = nullptr;
        progressBar = nullptr;
    }
}

UIConfig LinuxUserInterface::getConfig() {
    return _config;
}

void LinuxUserInterface::setConfig(const UIConfig& config) {
    _config = config;
} 
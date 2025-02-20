#include "LinuxPolishSystem.h"
#include "LinuxUserInterface.h"
#include "LinuxResourceManager.h"
#include <gtk/gtk.h>
#include <thread>
#include <chrono>

LinuxPolishSystem* LinuxPolishSystem::getInstance() {
    static LinuxPolishSystem instance;
    return &instance;
}

void LinuxPolishSystem::setTheme(const std::string& theme) {
    GtkSettings* settings = gtk_settings_get_default();
    g_object_set(G_OBJECT(settings), 
                "gtk-theme-name", theme.c_str(),
                "gtk-application-prefer-dark-theme", theme == "dark",
                nullptr);
}

void LinuxPolishSystem::setAnimationSpeed(float speed) {
    _animationSpeed = speed;
    GtkSettings* settings = gtk_settings_get_default();
    g_object_set(G_OBJECT(settings),
                "gtk-enable-animations", speed > 0.0f,
                nullptr);
}

void LinuxPolishSystem::enableTransitions(bool enable) {
    _transitionsEnabled = enable;
    // Apply transition settings to all windows
    GList* windows = gtk_window_list_toplevels();
    while (windows) {
        GtkWidget* window = GTK_WIDGET(windows->data);
        gtk_widget_set_opacity(window, enable ? 1.0 : 1.0);
        windows = windows->next;
    }
    g_list_free(windows);
}

void LinuxPolishSystem::showLoadingScreen(const LoadingScreen& screen) {
    _currentLoadingScreen = screen;
    
    GtkWidget* loadingWindow = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_position(GTK_WINDOW(loadingWindow), GTK_WIN_POS_CENTER);
    
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(loadingWindow), box);
    
    if (!screen.backgroundImage.empty()) {
        GtkWidget* image = gtk_image_new_from_file(screen.backgroundImage.c_str());
        gtk_box_pack_start(GTK_BOX(box), image, TRUE, TRUE, 0);
    }
    
    GtkWidget* label = gtk_label_new(screen.message.c_str());
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    
    if (screen.showSpinner) {
        GtkWidget* spinner = gtk_spinner_new();
        gtk_spinner_start(GTK_SPINNER(spinner));
        gtk_box_pack_start(GTK_BOX(box), spinner, FALSE, FALSE, 0);
    }
    
    GtkWidget* progressBar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), screen.progress);
    gtk_box_pack_start(GTK_BOX(box), progressBar, FALSE, FALSE, 0);
    
    gtk_widget_show_all(loadingWindow);
}

void LinuxPolishSystem::updateLoadingProgress(float progress) {
    _currentLoadingScreen.progress = progress;
    // Update progress bar in loading screen
    // This would need to be implemented with proper GTK widget references
}

void LinuxPolishSystem::hideLoadingScreen() {
    // Find and destroy loading screen window
    GList* windows = gtk_window_list_toplevels();
    while (windows) {
        GtkWidget* window = GTK_WIDGET(windows->data);
        if (GTK_IS_WINDOW(window) && 
            gtk_window_get_window_type(GTK_WINDOW(window)) == GTK_WINDOW_POPUP) {
            gtk_widget_destroy(window);
            break;
        }
        windows = windows->next;
    }
    g_list_free(windows);
}

void LinuxPolishSystem::setErrorStyle(const std::string& style) {
    // Apply custom CSS for error dialogs
    GtkCssProvider* provider = gtk_css_provider_new();
    std::string css = ".error-dialog { background-color: " + style + "; }";
    gtk_css_provider_load_from_data(provider, css.c_str(), -1, nullptr);
    
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
}

void LinuxPolishSystem::showDetailedError(
    const std::string& title,
    const std::string& message,
    const std::string& details
) {
    GtkWidget* dialog = gtk_message_dialog_new(
        nullptr,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_CLOSE,
        "%s", title.c_str()
    );
    
    gtk_message_dialog_format_secondary_markup(
        GTK_MESSAGE_DIALOG(dialog),
        "%s\n\n<small>%s</small>",
        message.c_str(),
        details.c_str()
    );
    
    if (_transitionsEnabled) {
        gtk_widget_set_opacity(dialog, 0.0);
        gtk_widget_show(dialog);
        
        // Fade in animation
        for (double i = 0.0; i <= 1.0; i += 0.1) {
            gtk_widget_set_opacity(dialog, i);
            while (gtk_events_pending()) gtk_main_iteration();
            std::this_thread::sleep_for(
                std::chrono::milliseconds(static_cast<int>(20 * _animationSpeed))
            );
        }
    }
    
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void LinuxPolishSystem::registerCleanupTask(
    const std::string& name,
    std::function<void()> task
) {
    _cleanupTasks.emplace_back(name, task);
}

void LinuxPolishSystem::runCleanupTasks() {
    LoadingScreen screen{
        "Cleaning up resources...",
        0.0f,
        "",
        true
    };
    showLoadingScreen(screen);
    
    float progress = 0.0f;
    float step = 1.0f / _cleanupTasks.size();
    
    for (const auto& task : _cleanupTasks) {
        task.second();
        progress += step;
        updateLoadingProgress(progress);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    hideLoadingScreen();
    _cleanupTasks.clear();
}

void LinuxPolishSystem::optimizeResources() {
    auto resourceMgr = LinuxResourceManager::getInstance();
    
    // Cleanup unused resources
    resourceMgr->cleanupUnusedResources();
    
    // Optimize memory usage
    resourceMgr->optimizeMemoryUsage();
    
    // Clear caches if memory usage is high
    if (resourceMgr->getCurrentMemoryUsage() > 
        resourceMgr->getMemoryLimit() * 0.8) {
        resourceMgr->clearCache();
    }
} 
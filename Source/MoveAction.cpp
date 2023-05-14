#include "MoveAction.h"
#include "2d/CCActionInterval.h"
#include "BaseGameLayer.h"
#include "GameToolbox.h"

MoveAction* MoveAction::create(float duration, const ax::Vec2& deltaPosition, GroupProperties* group)
{
	MoveAction* ret = new MoveAction();

	if (ret->initWithDuration(duration, deltaPosition, group))
	{
		ret->autorelease();

		return ret;
	}

	delete ret;
	return nullptr;
}

bool MoveAction::initWithDuration(float duration, const ax::Vec2& deltaPosition, GroupProperties* group)
{
	_group = group;
	_posDelta = deltaPosition;
	return MoveBy::initWithDuration(duration, deltaPosition);
}

void MoveAction::startWithTarget(ax::Node* target)
{
	ActionInterval::startWithTarget(target);
	_prevPositions.clear();
	for (auto obj : _group->_objects)
	{
		_prevPositions.push_back(obj->getPosition());
		_startPositions.push_back(obj->getPosition());
	}
	GameToolbox::log("action start");
}

ax::Vec2 currentPos, diff, newPos;

void MoveAction::update(float time)
{
#if AX_ENABLE_STACKABLE_ACTIONS
	size_t i = 0;
	for (auto obj : _group->_objects)
	{
		currentPos = obj->getPosition();
		diff = currentPos - _prevPositions[i];
		_startPositions[i] = _startPositions[i] + diff;
		newPos = _startPositions[i] + (_posDelta * time);
		obj->setPosition(newPos);
		_prevPositions[i] = newPos;
		auto section = BaseGameLayer::sectionForPos(newPos.x);
		section = section - 1 < 0 ? 0 : section - 1;
		if (obj->_section != section)
		{
			auto bgl = BaseGameLayer::getInstance();
			if(bgl->_sectionObjects.size() > obj->_section)
            {
                auto vec = &(bgl->_sectionObjects[obj->_section]);
                vec->erase(std::remove_if(vec->begin(), vec->end(), [&](GameObject* a) { return a == obj; }), vec->end());
                while (section >= bgl->_sectionObjects.size())
                {
                    std::vector<GameObject*> vec;
                    bgl->_sectionObjects.push_back(vec);
                }
                bgl->_sectionObjects[section].push_back(obj);
                obj->_section = section;
            }
		}
		i++;
	}
#else
	for (auto obj : _group->_objects)
		_obj->setPosition3D(_startPosition + _positionDelta * t);
#endif // AX_ENABLE_STACKABLE_ACTIONS
}

void MoveAction::stop()
{
	MoveBy::stop();
	_prevPositions.clear();
	_startPositions.clear();
}
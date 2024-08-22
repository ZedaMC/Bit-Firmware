#ifndef BIT_FIRMWARE_ACHIEVEMENTPOPUP_H
#define BIT_FIRMWARE_ACHIEVEMENTPOPUP_H

#include "LV_Interface/LVModal.h"
#include "Services/AchievementSystem.h"
#include "LV_Interface/LVStyle.h"

class AchievementPopup : public LVModal {
public:
	explicit AchievementPopup(LVScreen* parent, AchievementData achievementData);

private:
	const AchievementData achievementData;

	LVStyle defaultStyle;
	LVStyle debugStyle;

	void buildUI();
};


#endif //BIT_FIRMWARE_ACHIEVEMENTPOPUP_H

#include "AwardsScreen.h"
#include "Devices/Input.h"
#include "LV_Interface/InputLVGL.h"
#include "UIThread.h"
#include "Util/Services.h"
#include "HighScoreScreen.h"
#include "Periph/NVSFlash.h"
#include <sstream>
#include "Util/stdafx.h"
#include "Services/XPSystem.h"
#include "GameMenuScreen.h"
#include "Screens/XPBar.h"
#include "Util/Notes.h"
#include "Settings/Settings.h"
#include "Filepaths.hpp"
#include "Screens/Profile/AchievementView.h"

AwardsScreen::AwardsScreen(Games current, uint32_t highScore, uint32_t xp, std::vector<AchievementData>& achievements) :
		highScore(highScore), xp(xp), achievements(std::move(achievements)), currentGame(current), start(millis()){
	const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(xp > 0){
		setAwardMode(Award::XP);
	}else if(!this->achievements.empty()){
		setAwardMode(Award::Achievement);
	}else if(hsm->isHighScore(currentGame, highScore)){
		setAwardMode(Award::HighScore);
	}else{
		markForExit();
	}
}

void AwardsScreen::setAwardMode(Award award){
	lastChange = millis();
	chirped = false;

	const Settings* settings = (Settings*) Services.get(Service::Settings);
	if(settings == nullptr){
		return;
	}

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(levelSet == 0){
		levelSet = xpSystem->getLevel();
	}else if(award == Award::LevelUp){
		++levelSet;
	}

	ChirpSystem* chirp = (ChirpSystem*) Services.get(Service::Audio);
	if(chirp == nullptr){
		return;
	}

	if(award == Award::LevelUp){
		chirp->play({{ NOTE_C6, NOTE_C6, 90 },
					 { 0, 0,             20 },
					 { NOTE_C6, NOTE_C6, 90 },
					 { NOTE_G6, NOTE_G6, 200 }});
	}else if(award == Award::HighScore){
		chirp->play({{ NOTE_G6, NOTE_G6, 100 },
					 { NOTE_C6, NOTE_C6, 300 }});
	}else if(award == Award::XP){
		chirp->play({{ NOTE_C3, NOTE_C6, AnimLength }});
	}

	lv_obj_clean(*this);
	lv_obj_invalidate(*this);
	itemStyle = {};

	auto bg = lv_img_create(*this);
	lv_img_set_src(bg, THEMED_FILE(Background, settings->get().theme));

	auto bgSmall = lv_img_create(*this);
	lv_img_set_src(bgSmall, Filepath::Award::BackgroundSmall);
	lv_obj_align(bgSmall, LV_ALIGN_CENTER, 0, 0);

	rest = lv_obj_create(*this);
	lv_obj_set_size(rest, 108, 82);
	lv_obj_set_flex_flow(rest, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(rest, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_align(rest, LV_ALIGN_CENTER);

	if(award == Award::HighScore){
		lv_obj_set_style_pad_all(rest, 3, 0);

		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_border_width(itemStyle, 1);
		lv_style_set_border_color(itemStyle, lv_color_make(85, 126, 150));
		lv_style_set_border_opa(itemStyle, LV_OPA_COVER);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		lv_obj_t* enterImg = lv_img_create(rest);
		lv_img_set_src(enterImg, "S:/Award/highscore.bin");

		auto label = lv_label_create(rest);
		lv_label_set_text_fmt(label, "SCORE: %d", (int) highScore);
		lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_center(label);
		lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
		lv_obj_set_align(label, LV_ALIGN_CENTER);
		lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

		item = lv_obj_create(rest);
		lv_obj_add_style(item, itemStyle, 0);

		lv_obj_set_align(item, LV_ALIGN_CENTER);
		lv_obj_set_flex_flow(item, LV_FLEX_FLOW_ROW);
		lv_obj_set_flex_align(item, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

		name[0] = lv_label_create(item);
		lv_label_set_text(name[0], (std::stringstream() << characters[0]).str().c_str());
		lv_obj_set_size(name[0], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[0], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[0], 1, 0);
		lv_obj_set_style_text_color(name[0], lv_color_make(85, 126, 150), 0);

		name[1] = lv_label_create(item);
		lv_label_set_text(name[1], (std::stringstream() << characters[1]).str().c_str());
		lv_obj_set_size(name[1], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[1], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[1], 1, 0);
		lv_obj_set_style_text_color(name[1], lv_color_make(85, 126, 150), 0);

		name[2] = lv_label_create(item);
		lv_label_set_text(name[2], (std::stringstream() << characters[2]).str().c_str());
		lv_obj_set_size(name[2], LV_SIZE_CONTENT, LV_SIZE_CONTENT);
		lv_obj_set_style_border_color(name[2], lv_color_make(85, 126, 150), 0);
		lv_obj_set_style_border_width(name[2], 1, 0);
		lv_obj_set_style_text_color(name[2], lv_color_make(85, 126, 150), 0);

		lv_obj_add_event_cb(label, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			if(!awardsScreen->nextPart()){
				awardsScreen->markForExit();
				return;
			}
		}, LV_EVENT_CLICKED, this);

		lv_obj_add_event_cb(label, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			awardsScreen->markForExit();
		}, LV_EVENT_CANCEL, this);


		lv_group_add_obj(inputGroup, label);
		lv_group_set_editing(inputGroup, true);

		lv_obj_add_event_cb(label, [](lv_event_t* e){
			auto _this = (AwardsScreen*) e->user_data;
			auto key = lv_event_get_key(e);
			if(key == LV_KEY_LEFT){
				_this->activeIndex = _this->activeIndex == 0 ? 2 : _this->activeIndex - 1;
			}else if(key == LV_KEY_RIGHT){
				_this->activeIndex = (_this->activeIndex + 1) % 3;
			}else if(key == LV_KEY_UP || key == LV_KEY_DOWN){
				if(key == LV_KEY_UP){
					_this->characters[_this->activeIndex] = getChar(_this->characters[_this->activeIndex] + 1, 1);
				}else{
					_this->characters[_this->activeIndex] = getChar(_this->characters[_this->activeIndex] - 1, -1);
				}

				for(size_t i = 0; i < 3; ++i){
					lv_label_set_text(_this->name[i], (std::stringstream() << _this->characters[i]).str().c_str());
				}
			}
		}, LV_EVENT_KEY, this);

	}else if(award == Award::XP){
		lv_obj_set_style_pad_all(rest, 6, 0);

		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		auto mkLabel = [this](const char* text){
			auto item = lv_obj_create(rest);
			lv_obj_add_style(item, itemStyle, 0);
			lv_obj_set_style_height(item, 13, 0);
			lv_obj_set_align(item, LV_ALIGN_CENTER);

			auto label = lv_label_create(item);
			lv_label_set_text(label, text);
			lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_center(label);
			lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
			lv_obj_set_align(label, LV_ALIGN_CENTER);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

			return item;
		};

		auto text = lv_img_create(rest);
		lv_img_set_src(text, Filepath::Award::XpGained);
		lv_obj_set_align(text, LV_ALIGN_CENTER);

		mkLabel(("+" + std::to_string(xp) + "XP").c_str());

		auto lvl = mkLabel(("Level " + std::to_string(xpSystem->getLevel())).c_str());
		lv_obj_set_style_pad_top(lvl, -3, 0);

		auto bar = lv_img_create(rest);
		lv_img_set_src(bar, Filepath::Award::XpFrame);
		lv_obj_set_align(bar, LV_ALIGN_CENTER);
		lv_obj_set_style_bg_img_src(bar, Filepath::Award::XpBackground, 0);
		lv_obj_set_style_bg_opa(bar, 100, 0);

		xpBar = new XPBar(XPBarLength::Long, bar, xpSystem->MapXPToLevel(xpSystem->getXP()).progress);

		const LevelProgress progress = xpSystem->MapXPToLevel(xpSystem->getXP() + xp);
		xpBar->setFill(progress.nextLvl > levelSet + 1 ? 1.0f : progress.progress, true);

		lv_obj_set_align(*xpBar, LV_ALIGN_CENTER);

		lv_obj_add_event_cb(*xpBar, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			if(!awardsScreen->nextPart()){
				awardsScreen->markForExit();
				return;
			}
		}, LV_EVENT_CLICKED, this);

		lv_obj_add_event_cb(*xpBar, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			awardsScreen->markForExit();
		}, LV_EVENT_CANCEL, this);

		lv_group_add_obj(inputGroup, *xpBar);

	}else if(award == Award::LevelUp){
		lv_obj_set_style_pad_all(rest, 6, 0);

		lv_style_set_width(itemStyle, lv_pct(86));
		lv_style_set_height(itemStyle, 16);
		lv_style_set_radius(itemStyle, 2);
		lv_style_set_align(itemStyle, LV_ALIGN_CENTER);

		auto mkLabel = [this](const char* text){
			auto item = lv_obj_create(rest);
			lv_obj_add_style(item, itemStyle, 0);
			lv_obj_set_style_height(item, 13, 0);
			lv_obj_set_align(item, LV_ALIGN_CENTER);

			auto label = lv_label_create(item);
			lv_label_set_text(label, text);
			lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
			lv_obj_center(label);
			lv_obj_set_style_text_color(label, lv_color_make(85, 126, 150), 0);
			lv_obj_set_align(label, LV_ALIGN_CENTER);
			lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);

			return item;
		};

		auto text = lv_img_create(rest);
		lv_img_set_src(text, Filepath::Award::LeveledUp);
		lv_obj_set_align(text, LV_ALIGN_CENTER);

		auto lvl = mkLabel(("Level " + std::to_string(levelSet)).c_str());

		auto bar = lv_img_create(rest);
		lv_img_set_src(bar, Filepath::Award::XpFrame);
		lv_obj_set_align(bar, LV_ALIGN_CENTER);
		lv_obj_set_style_bg_img_src(bar, Filepath::Award::XpBackground, 0);
		lv_obj_set_style_bg_opa(bar, 100, 0);

		xpBar = new XPBar(XPBarLength::Long, bar, 0.0f);
		lv_obj_set_align(*xpBar, LV_ALIGN_CENTER);

		lv_obj_add_event_cb(*xpBar, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			if(!awardsScreen->nextPart()){
				awardsScreen->markForExit();
				return;
			}
		}, LV_EVENT_CLICKED, this);

		lv_obj_add_event_cb(*xpBar, [](lv_event_t* e){
			auto awardsScreen = (AwardsScreen*) e->user_data;

			awardsScreen->markForExit();
		}, LV_EVENT_CANCEL, this);

		lv_group_add_obj(inputGroup, *xpBar);

	}else if(award == Award::Achievement){
		lv_obj_set_style_pad_top(rest, 3, 0);

		auto img = lv_img_create(rest);
		lv_img_set_src(img, Filepath::Award::AchievementsUnlocked);
		lv_obj_set_align(img, LV_ALIGN_TOP_MID);

		lv_group_set_editing(inputGroup, true);

		achView = new AchievementView(this, rest, 4, 90, 27, achievements);

		lv_obj_set_pos(*achView, 17, 78);

		achView->select();
		achView->setReturnFunc([this](){
			if(!nextPart()){
				markForExit();
			}
		});
	}else{
		markForExit();
	}

	awardMode = award;
}

void AwardsScreen::onStart(){
	if(awardMode == Award::Achievement){
		achView->select();
	}else if(awardMode == Award::HighScore){
		InputLVGL::getInstance()->setVertNav(true);
	}
}

void AwardsScreen::onStop(){
	InputLVGL::getInstance()->setVertNav(false);

	if(XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem)){
		xpSystem->increment(xp);
	}

	HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	if(!hsm->isHighScore(currentGame, highScore)){
		return;
	}

	HighScore score = { .score = highScore };
	score.id[0] = characters[0];
	score.id[1] = characters[1];
	score.id[2] = characters[2];

	hsm->saveScore(currentGame, score);
}

void AwardsScreen::loop(){
	if(exitFlag){
		exitFlag = false;
		exit();
		return;
	}

	const XPSystem* xpSystem = (XPSystem*) Services.get(Service::XPSystem);
	if(xpSystem == nullptr){
		return;
	}

	if(awardMode == Award::LevelUp && millis() - lastChange >= 400 && !chirped){
		chirped = true;

		if(xpBar != nullptr){
			xpBar->setFill(XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl > levelSet + 1 ? 1.0f : xpSystem->MapXPToLevel(xpSystem->getXP() + xp).progress, true);
		}

		if(ChirpSystem* chirp = (ChirpSystem*) Services.get(Service::Audio)){
			chirp->play({{ NOTE_C3, NOTE_C6, AnimLength }});
		}
	}

	if(awardMode <= Award::LevelUp && millis() - lastChange >= AnimLength * 3 && XPSystem::MapXPToLevel(xpSystem->getXP() + xp).nextLvl > levelSet + 1){
		setAwardMode(Award::LevelUp);
	}

	if(awardMode == Award::HighScore){
		const uint64_t interval = (millis() - start) / blinkTime;

		if(interval % 2 == 0){
			for(size_t i = 0; i < 3; ++i){
				lv_obj_set_style_border_opa(name[i], 0, 0);
			}
		}else if(interval % 2 == 1){
			for(size_t i = 0; i < 3; ++i){
				if(i == activeIndex){
					lv_obj_set_style_border_opa(name[i], 100, 0);
				}else{
					lv_obj_set_style_border_opa(name[i], 0, 0);
				}
			}
		}
	}
}

void AwardsScreen::exit(){
	auto ui = (UIThread*) Services.get(Service::UI);
	if(ui == nullptr){
		return;
	}

	const HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return;
	}

	if(hsm->isHighScore(currentGame, highScore)){
		ui->startScreen([this](){ return std::make_unique<HighScoreScreen>(currentGame); });
	}else{
		ui->startScreen([this](){ return std::make_unique<GameMenuScreen>(currentGame); });
	}
}

bool AwardsScreen::nextPart(){
	HighScoreManager* hsm = (HighScoreManager*) Services.get(Service::HighScore);
	if(hsm == nullptr){
		return false;
	}


	if(awardMode < Award::Achievement && !achievements.empty()){
		setAwardMode(Award::Achievement);
	}else if(awardMode < Award::HighScore && hsm->isHighScore(currentGame, highScore)){
		setAwardMode(Award::HighScore);
	}else{
		return false;
	}

	return true;
}

void AwardsScreen::markForExit(){
	exitFlag = true;
}

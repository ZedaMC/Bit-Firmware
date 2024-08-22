#include <string>
#include "AchievementPopup.h"
#include "Filepaths.hpp"
#include "Services/AchievementTextData.hpp"
#include "LV_Interface/Theme/theme.h"
#include "Settings/Settings.h"
#include "Util/Services.h"

AchievementPopup::AchievementPopup(LVScreen* parent, AchievementData achievementData) : LVModal(parent), achievementData(achievementData){
	lv_style_set_pad_all(defaultStyle, 1);
	lv_style_set_border_width(defaultStyle, 1);
	lv_style_set_border_color(defaultStyle, lv_color_black());
	lv_style_set_border_opa(defaultStyle, LV_OPA_TRANSP);

	lv_style_set_border_color(debugStyle, lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_border_opa(debugStyle, LV_OPA_COVER);
	lv_style_set_border_width(debugStyle, 1);

	buildUI();

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (AchievementPopup*) e->user_data;
		delete modal;
	}, LV_EVENT_CLICKED, this);

	lv_obj_add_event_cb(*this, [](lv_event_t* e){
		auto modal = (AchievementPopup*) e->user_data;
		delete modal;
	}, LV_EVENT_CANCEL, this);

	lv_group_add_obj(inputGroup, *this);
	lv_group_focus_obj(*this);
}

void AchievementPopup::buildUI(){
	const Settings* settings = (Settings*) Services.get(Service::Settings);

	lv_obj_set_layout(*this, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(*this, LV_FLEX_FLOW_COLUMN);
	lv_obj_set_flex_align(*this, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
	lv_obj_set_style_pad_gap(*this, 3, 0);
//	lv_obj_add_style(*this, debugStyle, LV_STATE_DEFAULT);

	lv_obj_t* header = lv_obj_create(*this);

	lv_obj_set_layout(header, LV_LAYOUT_FLEX);
	lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
	lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
//	lv_obj_add_style(header, debugStyle, LV_STATE_DEFAULT);
	lv_obj_set_size(header, lv_pct(100), LV_SIZE_CONTENT);
	lv_obj_set_style_pad_gap(header, 3, LV_STATE_DEFAULT);

	auto base = lv_obj_create(header);
	lv_obj_center(base);
	lv_obj_set_size(base, 22, 22);
	lv_obj_add_flag(base, LV_OBJ_FLAG_SCROLL_ON_FOCUS);
	lv_obj_clear_flag(base, LV_OBJ_FLAG_SCROLLABLE);
	lv_group_add_obj(inputGroup, base);
//	lv_obj_add_style(base, defaultStyle, 0);
	lv_obj_set_style_bg_img_src(base, "S:/Ach/bg.bin", LV_STATE_DEFAULT);

	auto img = lv_img_create(base);
	lv_img_set_src(img, AchivementFile(achievementData.achievementID));
	lv_obj_center(img);

	auto title = lv_label_create(header);
	lv_obj_set_width(title, lv_pct(75));
	lv_label_set_text(title, AchievementTextData[(uint32_t) achievementData.achievementID].title);
	lv_obj_set_style_text_color(title, THEMED_COLOR(Foreground, settings->get().theme), 0);
//	lv_obj_add_style(title, debugStyle, LV_STATE_DEFAULT);

	auto desc = lv_label_create(*this);
	lv_obj_set_size(desc, lv_pct(95), LV_SIZE_CONTENT);
	lv_label_set_text(desc, AchievementTextData[(uint32_t) achievementData.achievementID].description);
//	lv_obj_set_style_text_font(desc, &landerfont, 0);
	lv_obj_set_style_text_color(desc, THEMED_COLOR(Foreground, settings->get().theme), 0);
//	lv_obj_add_style(desc, debugStyle, LV_STATE_DEFAULT);

	auto progress = lv_label_create(*this);
	lv_obj_set_size(progress, lv_pct(95), LV_SIZE_CONTENT);
	lv_label_set_text_fmt(progress, "%d/%d", (int) achievementData.progress, (int) achievementData.goal);
//	lv_obj_set_style_text_font(progress, &landerfont, 0);
	lv_obj_set_style_text_color(progress, THEMED_COLOR(Foreground, settings->get().theme), 0);
//	lv_obj_add_style(progress, debugStyle, LV_STATE_DEFAULT);
}

#include "plugin.h"
#include <obs-module.h>

OBS_DECLARE_MODULE()

struct metaball {
	float x;
	float y;
	float vx;
	float vy;
} ;
struct data {
	gs_vertbuffer_t * cuadrado;
	/*gs_texture_t* text;*/
	gs_effect_t* effect;
	gs_eparam_t* paramMetaballsPos;
	gs_eparam_t* paramMetaballsCount;
	gs_eparam_t* paramMetaballsColor;
	gs_eparam_t* paramMetaballsIsoline;
	long long width;
	long long height;
	int metaballcount;
	uint32_t color;
	bool isoline;
	struct metaball* metaballs;
};

static const char *metaball_source_get_name(void *unused)
{
	UNUSED_PARAMETER(unused);
	return "Metaballs";
}

static void metaball_source_update(void *data, obs_data_t *settings) {
	struct data* context = data;
	context->width = obs_data_get_int(settings, "width");
	context->height = obs_data_get_int(settings, "height");
	context->metaballcount = obs_data_get_int(settings, "metaballcount");
	context->color = (uint32_t)obs_data_get_int(settings, "color");
	context->isoline = obs_data_get_bool(settings, "metaballisoline");

	if (context->metaballs)
		bfree(context->metaballs);
	srand((unsigned)time(NULL));
	context->metaballs = bmalloc(sizeof(struct metaball)*20);
	for (int i = 0; i < context->metaballcount; i++)
	{
		context->metaballs[i].x = rand() % context->width;
		context->metaballs[i].y = rand() % context->height;

		context->metaballs[i].vx = (rand() % 5) + .3;
		context->metaballs[i].vy = (rand() % 5) + .3;
	}

	obs_enter_graphics();

	gs_render_start(true);
	gs_vertex2f(0.0f, context->height);
	gs_vertex2f(0.0f, 0.0f);
	gs_vertex2f(context->width, context->height);
	gs_vertex2f(context->width, 0.0f);
	gs_color(0xFFFFFFFF);
	gs_color(0xFFFF00FF);
	gs_color(0xFF00FFFF);
	gs_color(0xFF0000FF);
	context->cuadrado = gs_render_save();

	obs_leave_graphics();
}

static void *metaball_source_create(obs_data_t *settings, obs_source_t *source)
{
	UNUSED_PARAMETER(source);
	struct data * context = bzalloc(sizeof(struct data));

	metaball_source_update(context, settings);
	
	
	obs_enter_graphics();
	char* effect_file = obs_module_file("metaball.effect");
	char* err = NULL;
	context->effect = gs_effect_create_from_file(effect_file, &err);
	if (!context->effect) {
		blog(LOG_ERROR, "ERROR LOADING EFFECT: %s", err);
	}
	obs_leave_graphics();

	context->paramMetaballsPos = gs_effect_get_param_by_name(context->effect, "metaballspos");
	context->paramMetaballsCount = gs_effect_get_param_by_name(context->effect, "metaballcount");
	context->paramMetaballsColor = gs_effect_get_param_by_name(context->effect, "color");
	context->paramMetaballsIsoline = gs_effect_get_param_by_name(context->effect, "isoline");

	if (!context->paramMetaballsPos || !context->paramMetaballsCount) {
		blog(LOG_ERROR, "MetaballsPos/Count err");
	}
	return context;
}

static void metaball_source_destroy(void *data) {
	bfree(data);
}

static void metaball_source_defaults(obs_data_t *settings)
{
	obs_data_set_default_int(settings, "width", 1920ll);
	obs_data_set_default_int(settings, "height", 1080ll);
	obs_data_set_default_int(settings, "metaballcount", 2ll);
	obs_data_set_default_int(settings, "color", 0xFFFFFFFF);
	obs_data_set_default_bool(settings, "metaballisoline", true);
}

static void metaball_source_show(void *data)
{

}

static void metaball_source_hide(void *data)
{

}

static uint32_t metaball_source_getwidth(void *data)
{
	struct data* context = data;
	//blog(LOG_ERROR, "WIDTH %u", (uint32_t)context->width);
	return (uint32_t)context->width;
}

static uint32_t metaball_source_getheight(void *data)
{
	struct data* context = data;
	//blog(LOG_ERROR, "HEIGHT %u", (uint32_t)context->height);
	return (uint32_t)context->height;
}
static void metaball_source_render(void *data, gs_effect_t *_)
{
	struct data* context = data;
	
	//obs_get_base_effect(OBS_EFFECT_SOLID);
	
	gs_effect_set_val(context->paramMetaballsPos, context->metaballs, 20 * 4 * sizeof(float));
	gs_effect_set_int(context->paramMetaballsCount, context->metaballcount);
	gs_effect_set_color(context->paramMetaballsColor, context->color);
	gs_effect_set_bool(context->paramMetaballsIsoline, context->isoline);
	
	//obs_source_draw(context->text, 0, 0, 0, 0, 0);
	while (gs_effect_loop(context->effect, "Draw")) {
		gs_load_vertexbuffer(context->cuadrado);
		gs_draw(GS_TRISTRIP, 0, 0);
	}

	gs_load_vertexbuffer(NULL);
}

static void metaball_source_tick(void *data, float seconds)
{
	struct data* context = data;
	for (int c = 0; c < context->metaballcount; c++) {
		context->metaballs[c].x += context->metaballs[c].vx;
		context->metaballs[c].y += context->metaballs[c].vy;

		if (context->metaballs[c].x < 0 || context->metaballs[c].x > context->width)
			context->metaballs[c].vx *= -1;

		if (context->metaballs[c].y < 0 || context->metaballs[c].y > context->height)
			context->metaballs[c].vy *= -1;
	}
}
static obs_properties_t *metaball_source_properties(void *data)
{
	obs_properties_t *props = obs_properties_create();
	obs_properties_add_int(props, "width", "width", 0, 4096, 1);
	obs_properties_add_int(props, "height", "height", 0, 2160, 1);
	obs_properties_add_int(props, "metaballcount", "metaballs", 1, 20, 1);
	obs_properties_add_bool(props, "metaballisoline", "isoline");
	obs_properties_add_color(props, "color", "color");
	return props;
}
struct obs_source_info metaball_source = {
	.id = "metaball_source",
	.type = OBS_SOURCE_TYPE_INPUT,
	.output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_CUSTOM_DRAW,
	.get_name = metaball_source_get_name,
	.create = metaball_source_create,
	.destroy = metaball_source_destroy,
	.update = metaball_source_update,
	.get_defaults = metaball_source_defaults,
	.show = metaball_source_show,
	.hide = metaball_source_hide,
	.get_width = metaball_source_getwidth,
	.get_height = metaball_source_getheight,
	.video_render = metaball_source_render,
	.video_tick = metaball_source_tick,
	.get_properties = metaball_source_properties
};
bool obs_module_load(void)
{
	obs_register_source(&metaball_source);
	return true;
}
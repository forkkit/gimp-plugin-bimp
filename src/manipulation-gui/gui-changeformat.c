#include <gtk/gtk.h>
#include <libgimp/gimp.h>
#include "gui-changeformat.h"
#include "../bimp-manipulations.h"

static void update_frame_params(GtkComboBox*, changeformat_settings);
static void adv_expanded (GtkExpander *, gpointer);
static void set_dialog_size(gboolean);

GtkWidget *frame_params, *inner_widget;
GtkWidget *combo_format, *scale_quality, *scale_smoothing, *check_interlace, *scale_compression, *check_baseline;
GtkWidget *check_rle, *check_progressive, *check_entrophy, *combo_compression, *spin_markers, *combo_subsampling, *combo_dct;

GtkTextBuffer *buffer_comment;
GtkTextIter start_comment, end_comment;

GtkWidget* parentwin;

GtkWidget* bimp_changeformat_gui_new(changeformat_settings settings, GtkWidget* parent)
{
	GtkWidget *gui;
	
	parentwin = parent;
	gui = gtk_vbox_new(FALSE, 5);
	
	combo_format = gtk_combo_box_new_text();
	gtk_widget_set_size_request (combo_format, COMBO_FORMAT_W, COMBO_FORMAT_H);
	int i;
	for(i = 0; i < FORMAT_END; i++) {
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_format), format_type_string[i][1]);
	}
	gtk_combo_box_set_active(GTK_COMBO_BOX(combo_format), settings->format);
	
	frame_params = gtk_frame_new("Format settings:");
	gtk_widget_set_size_request (frame_params, FRAME_PARAMS_W, FRAME_PARAMS_H);
	
	gtk_box_pack_start(GTK_BOX(gui), combo_format, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(gui), frame_params, FALSE, FALSE, 0);
	
	update_frame_params(GTK_COMBO_BOX(combo_format), settings);	
	
	g_signal_connect(G_OBJECT(combo_format), "changed", G_CALLBACK(update_frame_params), settings);
	
	return gui;
}

static void update_frame_params(GtkComboBox *widget, changeformat_settings settings) 
{
	set_dialog_size(FALSE);
	format_type selected_format = (format_type)gtk_combo_box_get_active(widget);
	
	if (selected_format == FORMAT_GIF) {
		inner_widget = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(inner_widget), 8);
		check_interlace = gtk_check_button_new_with_label("Interlaced");
		
		if (selected_format == settings->format) {
			format_params_gif settings_gif = (format_params_gif)(settings->params);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_interlace), settings_gif->interlace);
		}
		else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_interlace), FALSE);
		}
		
		gtk_box_pack_start(GTK_BOX(inner_widget), check_interlace, FALSE, FALSE, 0);
	} 
	else if (selected_format == FORMAT_JPEG) {
		GtkWidget *hbox_quality, *hbox_smoothing, *hbox_checks, *hbox_comment, *hbox_markers, *hbox_subsampling, *hbox_dct;
		GtkWidget *expander_advanced, *vbox_advanced, *label_quality, *label_smoothing, *label_markers, *label_comment, *label_subsampling, *label_dct, *text_comment;
		
		inner_widget = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(inner_widget), 8);
		
		hbox_quality = gtk_hbox_new(FALSE, 5);
		label_quality = gtk_label_new("Quality");
		gtk_widget_set_size_request (label_quality, LABEL_QUALITY_W, LABEL_QUALITY_H);
		gtk_misc_set_alignment(GTK_MISC(label_quality), 0.5, 0.8);
		scale_quality = gtk_hscale_new_with_range(0, 100, 1);
		gtk_widget_set_size_request (scale_quality, SCALE_QUALITY_W, SCALE_QUALITY_H);
		
		expander_advanced = gtk_expander_new("Advanced params");
		vbox_advanced = gtk_vbox_new(FALSE, 5);
		
		hbox_smoothing = gtk_hbox_new(FALSE, 5);
		label_smoothing = gtk_label_new("Smoothing");
		gtk_widget_set_size_request (label_smoothing, LABEL_QUALITY_W, LABEL_QUALITY_H);
		gtk_misc_set_alignment(GTK_MISC(label_smoothing), 0.5, 0.8);
		scale_smoothing = gtk_hscale_new_with_range(0, 1, 0.01);
		gtk_widget_set_size_request (scale_smoothing, SCALE_QUALITY_W, SCALE_QUALITY_H);
		
		hbox_checks = gtk_hbox_new(FALSE, 5);
		check_entrophy = gtk_check_button_new_with_label("Optimize");
		check_progressive = gtk_check_button_new_with_label("Progressive");
		check_baseline = gtk_check_button_new_with_label("Save baseline");
		
		hbox_comment = gtk_hbox_new(FALSE, 5);
		label_comment = gtk_label_new("Comment:");
		gtk_widget_set_size_request (label_comment, LABEL_QUALITY_W, LABEL_QUALITY_H);
		text_comment =  gtk_text_view_new();
		buffer_comment = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_comment));
		gtk_widget_set_size_request (text_comment, TEXT_COMMENT_W, TEXT_COMMENT_H);
		
		hbox_markers = gtk_hbox_new(FALSE, 5);
		label_markers = gtk_label_new("Markers rows:");
		gtk_widget_set_size_request (label_markers, LABEL_QUALITY_W, LABEL_QUALITY_H);
		spin_markers = gtk_spin_button_new(NULL, 1, 0);
		
		hbox_subsampling = gtk_hbox_new(FALSE, 5);
		label_subsampling = gtk_label_new("Subsampling:");
		gtk_widget_set_size_request (label_subsampling, LABEL_QUALITY_W, LABEL_QUALITY_H);
		combo_subsampling = gtk_combo_box_new_text();
		gtk_widget_set_size_request (combo_subsampling, COMBO_COMPRESSION_W, COMBO_COMPRESSION_H);
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_subsampling), "1x1, 1x1, 1x1 (quality)");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_subsampling), "2x1, 1x1, 1x1 (4:2:2)");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_subsampling), "1x2, 1x1, 1x1");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_subsampling), "2x2, 1x1, 1x1 (small size)");
		
		hbox_dct = gtk_hbox_new(FALSE, 5);
		label_dct = gtk_label_new("DCT algorithm:");
		gtk_widget_set_size_request (label_dct, LABEL_QUALITY_W, LABEL_QUALITY_H);
		combo_dct = gtk_combo_box_new_text();
		gtk_widget_set_size_request (combo_dct, COMBO_COMPRESSION_W, COMBO_COMPRESSION_H);
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_dct), "Fast integer");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_dct), "Integer");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_dct), "Float");
		
		if (selected_format == settings->format) {
			format_params_jpeg settings_jpeg = (format_params_jpeg)(settings->params);
			gtk_range_set_value(GTK_RANGE(scale_quality), settings_jpeg->quality);
			gtk_range_set_value(GTK_RANGE(scale_smoothing), settings_jpeg->smoothing);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_entrophy), settings_jpeg->entropy);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_progressive), settings_jpeg->progressive);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_baseline), settings_jpeg->baseline);
			
			gtk_spin_button_configure (GTK_SPIN_BUTTON(spin_markers), GTK_ADJUSTMENT(gtk_adjustment_new (settings_jpeg->markers, 0, 64, 1, 1, 0)), 0, 0);
			
			buffer_comment = gtk_text_view_get_buffer(GTK_TEXT_VIEW (text_comment));
			gtk_text_buffer_set_text (buffer_comment, settings_jpeg->comment, -1); 
			gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_comment), buffer_comment);
			
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_subsampling), settings_jpeg->subsampling);
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_dct), settings_jpeg->dct);
		}
		else {
			gtk_range_set_value(GTK_RANGE(scale_quality), 85.0);
			gtk_range_set_value(GTK_RANGE(scale_smoothing), 0.0);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_entrophy), TRUE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_progressive), FALSE);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_baseline), FALSE);
			
			gtk_spin_button_configure (GTK_SPIN_BUTTON(spin_markers), GTK_ADJUSTMENT(gtk_adjustment_new (0, 0, 64, 1, 1, 0)), 0, 0);
			
			gtk_text_buffer_set_text (buffer_comment, "", -1); 
			gtk_text_view_set_buffer(GTK_TEXT_VIEW(text_comment), buffer_comment);
			
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_subsampling), 3);
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_dct), 1);
		}
		
		gtk_box_pack_start(GTK_BOX(hbox_quality), label_quality, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_quality), scale_quality, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(inner_widget), hbox_quality, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(inner_widget), expander_advanced, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(hbox_smoothing), label_smoothing, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_smoothing), scale_smoothing, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_smoothing, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(hbox_checks), check_entrophy, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_checks), check_progressive, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_checks), check_baseline, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_checks, FALSE, FALSE, 0);
				
		gtk_box_pack_start(GTK_BOX(hbox_comment), label_comment, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_comment), text_comment, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_comment, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(hbox_markers), label_markers, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_markers), spin_markers, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_markers, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(hbox_subsampling), label_subsampling, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_subsampling), combo_subsampling, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_subsampling, FALSE, FALSE, 0);
		
		gtk_box_pack_start(GTK_BOX(hbox_dct), label_dct, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_dct), combo_dct, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(vbox_advanced), hbox_dct, FALSE, FALSE, 0);
		
		gtk_container_add (GTK_CONTAINER(expander_advanced), vbox_advanced);
		
		g_signal_connect(G_OBJECT(expander_advanced), "activate", G_CALLBACK(adv_expanded), NULL);
	}
	else if (selected_format == FORMAT_PNG) {
		GtkWidget *hbox_compression, *label_compression;
		
		inner_widget = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(inner_widget), 8);
		check_interlace = gtk_check_button_new_with_label("Interlace (Adam7)");
		hbox_compression = gtk_hbox_new(FALSE, 5);
		label_compression = gtk_label_new("Compression");
		gtk_widget_set_size_request (label_compression, LABEL_COMPRESSION_W, LABEL_COMPRESSION_H);
		gtk_misc_set_alignment(GTK_MISC(label_compression), 0.5, 0.8);
		scale_compression = gtk_hscale_new_with_range(0, 9, 1);
		gtk_widget_set_size_request (scale_compression, SCALE_COMPRESSION_W, SCALE_COMPRESSION_H);
		
		if (selected_format == settings->format) {
			format_params_png settings_png = (format_params_png)(settings->params);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_interlace), settings_png->interlace);
			gtk_range_set_value(GTK_RANGE(scale_compression), settings_png->compression);
		}
		else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_interlace), FALSE);
			gtk_range_set_value(GTK_RANGE(scale_compression), 9);
		}
		
		gtk_box_pack_start(GTK_BOX(inner_widget), check_interlace, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_compression), label_compression, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_compression), scale_compression, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(inner_widget), hbox_compression, FALSE, FALSE, 0);
	}
	else if (selected_format == FORMAT_TGA) {		
		inner_widget = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(inner_widget), 8);
		check_rle = gtk_check_button_new_with_label("RLE compression");
		
		if (selected_format == settings->format) {
			format_params_tga settings_tga = (format_params_tga)(settings->params);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_rle), settings_tga->rle);
		}
		else {
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_rle), FALSE);
		}
		
		gtk_box_pack_start(GTK_BOX(inner_widget), check_rle, FALSE, FALSE, 0);
	}
	else if (selected_format == FORMAT_TIFF) {
		GtkWidget *hbox_compression, *label_compression;
		
		inner_widget = gtk_vbox_new(FALSE, 5);
		gtk_container_set_border_width(GTK_CONTAINER(inner_widget), 8);
		hbox_compression = gtk_hbox_new(FALSE, 5);
		label_compression = gtk_label_new("Compression");
		gtk_widget_set_size_request (label_compression, LABEL_COMPRESSION_W, LABEL_COMPRESSION_H);
		gtk_misc_set_alignment(GTK_MISC(label_compression), 0.5, 0.5);
		combo_compression = gtk_combo_box_new_text();
		gtk_widget_set_size_request (combo_compression, COMBO_COMPRESSION_W, COMBO_COMPRESSION_H);
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_compression), "None");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_compression), "LZW");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_compression), "Pack bits");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_compression), "Deflate");
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo_compression), "JPEG");
		
		if (selected_format == settings->format) {
			format_params_tiff settings_tiff = (format_params_tiff)(settings->params);
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_compression), settings_tiff->compression);
		}
		else {
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo_compression), 0);
		}
		
		gtk_box_pack_start(GTK_BOX(hbox_compression), label_compression, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(hbox_compression), combo_compression, FALSE, FALSE, 0);
		gtk_box_pack_start(GTK_BOX(inner_widget), hbox_compression, FALSE, FALSE, 0);
	}
	else {
		inner_widget = gtk_label_new("This format has no params");
	}
	
	if (gtk_bin_get_child(GTK_BIN(frame_params)) != NULL) {
		gtk_widget_destroy(gtk_bin_get_child(GTK_BIN(frame_params)));
	}
	gtk_container_add(GTK_CONTAINER(frame_params), inner_widget);
	gtk_widget_show_all(frame_params);
}

static void adv_expanded (GtkExpander *expander, gpointer data)
{
	if (!gtk_expander_get_expanded (GTK_EXPANDER(expander))) {
		set_dialog_size(TRUE);
	}
	else {
		set_dialog_size(FALSE);
	}
}

static void set_dialog_size(gboolean expanded) {
	if (expanded) {
		gtk_widget_set_size_request (parentwin, CHANGEFORMAT_WINDOW_W, CHANGEFORMAT_WINDOW_H_EXPANDED);
		gtk_widget_set_size_request (frame_params, FRAME_PARAMS_W, FRAME_PARAMS_H_EXPANDED);
	}
	else {
		gtk_widget_set_size_request (parentwin, CHANGEFORMAT_WINDOW_W, CHANGEFORMAT_WINDOW_H);
		gtk_widget_set_size_request (frame_params, FRAME_PARAMS_W, FRAME_PARAMS_H);
	}
}

void bimp_changeformat_save(changeformat_settings orig_settings) 
{
	orig_settings->format = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_format));
	g_free(orig_settings->params);
	
	if (orig_settings->format == FORMAT_GIF) {
		orig_settings->params = (format_params_gif) g_malloc(sizeof(struct changeformat_params_gif));
		((format_params_gif)orig_settings->params)->interlace = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_interlace));
	}
	else if (orig_settings->format == FORMAT_JPEG) {
		orig_settings->params = (format_params_jpeg) g_malloc(sizeof(struct changeformat_params_jpeg));
		((format_params_jpeg)orig_settings->params)->quality = gtk_range_get_value(GTK_RANGE(scale_quality));
		((format_params_jpeg)orig_settings->params)->smoothing = gtk_range_get_value(GTK_RANGE(scale_smoothing));
		((format_params_jpeg)orig_settings->params)->entropy = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_entrophy));
		((format_params_jpeg)orig_settings->params)->progressive = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_progressive));
		((format_params_jpeg)orig_settings->params)->baseline = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_baseline));
		
		gtk_text_buffer_get_start_iter(buffer_comment, &start_comment);
        gtk_text_buffer_get_end_iter(buffer_comment, &end_comment);
		((format_params_jpeg)orig_settings->params)->comment = g_strdup(gtk_text_buffer_get_text(buffer_comment, &start_comment, &end_comment, TRUE));
		
		((format_params_jpeg)orig_settings->params)->markers = gtk_spin_button_get_value(GTK_SPIN_BUTTON(spin_markers));
		((format_params_jpeg)orig_settings->params)->subsampling = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_subsampling));
		((format_params_jpeg)orig_settings->params)->dct = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_dct));
	}
	else if (orig_settings->format == FORMAT_PNG) {
		orig_settings->params = (format_params_png) g_malloc(sizeof(struct changeformat_params_png));
		((format_params_png)orig_settings->params)->interlace = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_interlace));
		((format_params_png)orig_settings->params)->compression = gtk_range_get_value(GTK_RANGE(scale_compression));
	}
	else if (orig_settings->format == FORMAT_TGA) {
		orig_settings->params = (format_params_tga) g_malloc(sizeof(struct changeformat_params_tga));
		((format_params_tga)orig_settings->params)->rle = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_rle));
	}
	else if (orig_settings->format == FORMAT_TIFF) {
		orig_settings->params = (format_params_png) g_malloc(sizeof(struct changeformat_params_png));
		((format_params_tiff)orig_settings->params)->compression = gtk_combo_box_get_active(GTK_COMBO_BOX(combo_compression));
	}
	else {
		orig_settings->params = NULL;
	}
}




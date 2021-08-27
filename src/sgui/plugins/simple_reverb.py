# -*- coding: utf-8 -*-
"""
This file is part of the Stargate project, Copyright Stargate Team

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 3 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

"""

from sgui.widgets import *
from sglib.lib.translate import _


SREVERB_REVERB_TIME = 0
SREVERB_REVERB_WET = 1
SREVERB_REVERB_COLOR = 2
SREVERB_REVERB_DRY = 3
SREVERB_REVERB_PRE_DELAY = 4
SREVERB_REVERB_HP = 5


SREVERB_PORT_MAP = {
    "Reverb Wet": SREVERB_REVERB_WET,
    "Reverb Dry": SREVERB_REVERB_DRY,
    "Reverb LP": SREVERB_REVERB_COLOR,
    "Reverb HP": SREVERB_REVERB_HP,
}

STYLESHEET = """\
QWidget#plugin_window{
    background: qlineargradient(
        x1: 0, y1: 0, x2: 1, y2: 0,
        stop: 0 #1e9140, stop: 0.17 #1e7391,
        stop: 0.33 #1e9140, stop: 0.5 #1e7391,
        stop: 0.66 #1e9140, stop: 0.83 #1e7391,
        stop: 1 #1e9140
    );
}

QComboBox{
    background: qlineargradient(
        x1: 0, y1: 0, x2: 0, y2: 1,
        stop: 0 #6a6a6a, stop: 0.5 #828282, stop: 1 #6a6a6a
    );
    border: 1px solid #222222;
    border-radius: 6px;
    color: #cccccc;
}

QLabel#plugin_value_label,
QLabel#plugin_name_label
{
    background-color: none;
    border: none;
    color: #cccccc;
}

"""

class sreverb_plugin_ui(AbstractPluginUI):
    def __init__(self, *args, **kwargs):
        AbstractPluginUI.__init__(
            self,
            *args,
            stylesheet=STYLESHEET,
            **kwargs
        )
        self._plugin_name = "SREVERB"
        self.is_instrument = False

        self.preset_manager = None

        self.layout.setSizeConstraint(
            QLayout.SizeConstraint.SetFixedSize,
        )

        self.delay_hlayout = QHBoxLayout()
        self.layout.addLayout(self.delay_hlayout)

        f_knob_size = 75

        self.reverb_groupbox_gridlayout = QGridLayout()
        self.delay_hlayout.addLayout(self.reverb_groupbox_gridlayout)

        knob_kwargs = {
            'arc_width_pct': 20.,
            'fg_svg': None,
            'arc_brush': QColor('#4b3fb5')
        }
        self.reverb_time_knob = knob_control(
            f_knob_size,
            _("Size"),
            SREVERB_REVERB_TIME,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            0,
            100,
            50,
            KC_DECIMAL,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_time_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            3,
        )

        knob_kwargs['arc_brush'] = QColor("#9f3f98")
        self.reverb_dry_knob = knob_control(
            f_knob_size,
            _("Dry"),
            SREVERB_REVERB_DRY,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            -500,
            0,
            0,
            KC_TENTH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_dry_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            9,
        )

        knob_kwargs['arc_brush'] = QColor("#8a3f9f")
        self.reverb_wet_knob = knob_control(
            f_knob_size,
            _("Wet"),
            SREVERB_REVERB_WET,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            -500,
            0,
            -120,
            KC_TENTH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_wet_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            10,
        )

        knob_kwargs['arc_brush'] = QColor("#753fa6")
        self.reverb_hp_knob = knob_control(
            f_knob_size,
            _("HP"),
            SREVERB_REVERB_HP,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            20,
            96,
            50,
            KC_PITCH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_hp_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            15,
        )

        knob_kwargs['arc_brush'] = QColor("#b53f91")
        self.reverb_lp_knob = knob_control(
            f_knob_size,
            _("LP"),
            SREVERB_REVERB_COLOR,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            48,
            120,
            90,
            KC_PITCH,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_lp_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            16,
        )

        knob_kwargs['arc_brush'] = QColor("#603fad")
        self.reverb_predelay_knob = knob_control(
            f_knob_size,
            _("PreDelay"),
            SREVERB_REVERB_PRE_DELAY,
            self.plugin_rel_callback,
            self.plugin_val_callback,
            0,
            1000,
            10,
            KC_MILLISECOND,
            self.port_dict,
            self.preset_manager,
            knob_kwargs=knob_kwargs,
        )
        self.reverb_predelay_knob.add_to_grid_layout(
            self.reverb_groupbox_gridlayout,
            21,
        )

        self.open_plugin_file()
        self.set_midi_learn(SREVERB_PORT_MAP)

    def open_plugin_file(self):
        AbstractPluginUI.open_plugin_file(self)

    def save_plugin_file(self):
        # Don't allow the spectrum analyzer to run at startup
        AbstractPluginUI.save_plugin_file(self)

    def ui_message(self, a_name, a_value):
        AbstractPluginUI.ui_message(a_name, a_value)



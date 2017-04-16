/* 
 * RemoteSwitch library v2.2.1 (20120314) made by Randy Simons http://randysimons.nl/
 * See RemoteReceiver.h for details.
 *
 * License: GPLv3. See license.txt
 *
 * Ported to Raspberry PI (using PIGPIO) by helly
 * GPIO needs to be initialised in calling function .... 
 * v1.0.0 (20161224)
 *
 */

#include <stdint.h>
#include <pigpiod_if2.h>
#include "RemoteControl.h"

/************
* RemoteControl
************/

RemoteControl::RemoteControl(int pi, short enapin, short txrxpin, bool chargepll) {
	int retval = 0;
	_pi = pi;
	_enapin = enapin;
	_txrxpin = txrxpin;
	_chargepll = chargepll;
	_enabled = false;
	_tx = false;

	if (enapin > 0) {
		retval = set_mode(pi, enapin, PI_OUTPUT); // Set GPIOxx as output.
	}

	if (!retval) {
		if (enapin > 0) {
			retval = set_pull_up_down(pi, enapin, PI_PUD_OFF); // no pull up/ pull down
		}
	}

	if (!retval) {
		if (txrxpin > 0) {
			retval = set_mode(pi, txrxpin, PI_OUTPUT); // Set GPIOxx as output.
		}
	}

	if (!retval) {
		if (txrxpin > 0) {
			retval = set_pull_up_down(pi, txrxpin, PI_PUD_OFF); // no pull up/ pull down
		}
	}
	
	enable();

	_initok = retval? false:true;
}

void RemoteControl::enable() {
	if ((_initok) && (!_enabled)) {
		if (_chargepll) {
			enable2();
		} else {
			if (_txrxpin > 0) {
				gpio_write(_pi, _txrxpin, 0);
			}
			_tx = false;
			if (_enapin > 0) {
				gpio_write(_pi, _enapin, 1);
				_enabled = true;
				// It takes 20us to enable, let's wait for it.
				time_sleep(0.00002);
			} else {
				_enabled = true;
			}
		}
	}
}

void RemoteControl::disable() {
	if ((_initok) && (_enabled)) {
		if (_enapin > 0) {
			gpio_write(_pi, _enapin, 0);
		}
		_enabled = false;
	}
}

bool RemoteControl::isEnabled() {
	return _enabled;
}

void RemoteControl::Rx() {
	if ((_initok) && (_enabled) && (_tx)) {
		if (_chargepll) {
			Rx2();
		} else {
			if (_txrxpin > 0) {
				gpio_write(_pi, _txrxpin, 0);
				_tx = false;
				// It takes 200us for valid reception, let's wait for it.
				time_sleep(0.0002);
			} else {
				_tx = false;
			}
		}
	}
}

void RemoteControl::Tx() {
	if ((_initok) && (_enabled) && (!_tx)) {
		if (_txrxpin > 0) {
			gpio_write(_pi, _txrxpin, 1);
			_tx = true;
			// It takes 400us for valid transmission, let's wait for it.
			time_sleep(0.0004);
		} else {
			_tx = true;
		}
	}
}

bool RemoteControl::isTx() {
	return _tx;
}

void RemoteControl::enable2() {
	// make sure you are in _rx mode
	if ((_txrxpin > 0) && (_tx)) {
		gpio_write(_pi, _txrxpin, 0);
		_tx = false;
	}
	// enable
	if (_enapin > 0) {
		gpio_write(_pi, _enapin, 1);
	}
	_enabled = true;
	// wait 20us
	time_sleep(0.00002);
	// tx
	if (_txrxpin > 0) {
		gpio_write(_pi, _txrxpin, 1);
	}
	_tx = true;
	// wait 200us
	time_sleep(0.0002);

	Rx2();
}

void RemoteControl::Rx2() {
	// rx
	if (_txrxpin > 0) {
		gpio_write(_pi, _txrxpin, 0);
	}
	_tx = false;
	// wait 40us
	time_sleep(0.00004);
	// disable
	if (_enapin > 0) {
		gpio_write(_pi, _enapin, 0);
	}
	_enabled = false;
	// wait 20us
	time_sleep(0.00002);
	// enable
	if (_enapin > 0) {
		gpio_write(_pi, _enapin, 1);
	}
	_enabled = true;
	// It takes 200us for valid reception, let's wait for it.
	time_sleep(0.0002);
}

// access functions
extern "C" RemoteControl* _RemoteControl(int pi, short enapin, short txrxpin, bool chargepll) {
	return new RemoteControl(pi, enapin, txrxpin, chargepll);
}

extern "C" void _RemoteControl_delete(RemoteControl* t) {
	delete t;
}

extern "C" void _RemoteControl_enable(RemoteControl* t) {
	t->enable();
}

extern "C" void _RemoteControl_disable(RemoteControl* t) {
	t->disable();
}

extern "C" bool _RemoteControl_isEnabled(RemoteControl* t) {
	return t->isEnabled();
}

extern "C" void _RemoteControl_Rx(RemoteControl* t) {
	t->Rx();
}

extern "C" void _RemoteControl_Tx(RemoteControl* t) {
	t->Tx();
}

extern "C" bool _RemoteControl_isTx(RemoteControl* t) {
	return t->isTx();
}

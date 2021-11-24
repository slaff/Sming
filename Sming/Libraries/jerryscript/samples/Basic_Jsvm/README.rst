Basic_Jsvm
==========

.. highligh: JavaScript

This sample demonstrates to run JavaScript applications in a sandbox inside a microcontroller.
For JavaScript engine Sming uses JerryScript. 


The code::

	var x;

	/**
	 * This is a function that will be called once.
	 */
	function setup() {
		x = 0;
		print ('Setup: X='+x);
	}
	
	/**
	 * This is a function that will be called every half a second
	 */
	function loop() {
		x++;
		print('Loop: X='+x);
	}


TDBs
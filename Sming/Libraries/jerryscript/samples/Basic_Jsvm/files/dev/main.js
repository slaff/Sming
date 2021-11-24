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
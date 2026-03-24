// character.h
// A small 5-wide x 7-tall robot character for positioning demos
// Designed to be moved around the 12x8 matrix with setPosition()
//
// Frame 1 (standing):            Frame 2 (bounce):
//   .XXX........                   ............
//   .X.X........                   .XXX........
//   .XXX........                   .X.X........
//   XXXXX.......                   .XXX........
//   .XXX........                   XXXXX.......
//   .X.X........                   .XXX........
//   XX.XX.......                   ..X.........
//   ............                   .X.X........

const uint32_t character[][4] = {
	{
		0x70050070,
		0x0F807005,
		0x00D80000,
		300
	},
	{
		0x00070050,
		0x0700F807,
		0x00200500,
		300
	}
};

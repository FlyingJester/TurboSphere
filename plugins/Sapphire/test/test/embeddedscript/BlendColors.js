function BlendColors(c1, c2){
	return new Color(
		Math.floor((c1.red   + c2.red   ) /2),
		Math.floor((c1.green + c2.green ) /2),
		Math.floor((c1.blue  + c2.blue  ) /2),
		Math.floor((c1.alpha + c2.alpha ) /2)
	);
}
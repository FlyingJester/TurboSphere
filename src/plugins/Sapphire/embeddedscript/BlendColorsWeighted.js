function BlendColorsWeighted(c1, c2, w1, w2){

	var wt = w1+w2;

	return new Color(
		Math.floor(((c1.red   * w1) + (c2.red   * w2)) /wt),
		Math.floor(((c1.green * w1) + (c2.green * w2)) /wt),
		Math.floor(((c1.blue  * w1) + (c2.blue  * w2)) /wt),
		Math.floor(((c1.alpha * w1) + (c2.alpha * w2)) /wt)
	);
}
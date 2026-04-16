int main() {
	float arr[100][200];
	int i, j;
	float i_1, j_1;
	float max;
	int max_i, max_j;

	i_1 = 0.0;
	for (i = 0; i < 100; i = i + 1;) {
		j_1 = 0.0;
		for (j = 0; j < 200; j = j + 1;) {
			arr[i][j] = j_1 - (5.00 * i_1) + (j_1 / (j_1 + 1.0));
			j_1 = j_1 + 1.0;
		}
		i_1 = i_1 + 1.0;
	}

	max_i = -1;
	max_j = -1;
	max = -100000.000;
	for (i = 0; i < 100; i = i + 1;) {
		for (j = 0; j < 200; j = j + 1;) {
			if (arr[i][j] > max) {
				max = arr[i][j];
				max_i = i;
				max_j = j;
			}
		}
	}

	print "Max value: ";
	print max;
	print "\n";

	print "Max indices, i = ";
	print max_i;
	print ", j = ";
	print max_j;
	print "\n";

	return 0;
}
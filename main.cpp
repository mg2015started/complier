#include "PredictTable.cpp"

int main()
{
	PredictTable table("productions.txt");
	table.generateTable();
	return 0;
}
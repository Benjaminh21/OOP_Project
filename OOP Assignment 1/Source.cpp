/*
File: Source.cpp
Author: Benjamin Terrill
Created: 01/11/2018
*/

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;

//Function Prototypes
double* read_text(char *fileName, int sizeR, int sizeC);
void write_pgm(char *filename, double *data, int sizeR, int sizeC, int Q);
int* NNS_Algorithm(double* Base_Image, double* Ref_Image, int Base_Rows, int Base_Columns, int Ref_Rows, int Ref_Columns);

//Classes
class Base_Image {
private:

public:
};

class Ref_Image : public Base_Image{
	char Wally[20] = "Wally_grey.txt";
	char *FileName = Wally;
private:

public:
};

class Large_Image : public Base_Image {
	char Cluttered[16] = "Cluttered_Scene";
	char *FileName = Cluttered;
private:

public:
};

int main()
{
	cout << "Program Starting:" << endl;
	cout << " " << endl;
	cout << "Please Wait..." << endl;
	cout << " " << endl;

	char Wally[20] = "Wally_grey.txt";
	char Cluttered[20] = "Cluttered_scene.txt";
	char pgm_BaseImage[20] = "pgm_BaseImage.pgm";
	char pgm_RefImage[20] = "pgm_RefImage.pgm";
	char pgm_WallyFound[20] = "pgm_WallyFound.pgm";
	char *pgmFileName = pgm_BaseImage;
	char *pgmFileName2 = pgm_RefImage;
	char *pgmWallyFound = pgm_WallyFound;

	char *FileName = Cluttered;
	ifstream File(FileName);
	string Line, word;
	int Base_Rows = 0;
	int Base_Columns = 0;

	getline(File, Line);
	Base_Rows++;

	istringstream iss(Line);

	while (iss >> word) {
		Base_Columns++;
	}

	while (getline(File, Line)) {
		Base_Rows++;
	}

	//cout << "Rows: " << Base_Rows << endl;
	//cout << "Columns: " << Base_Columns << endl;

	double* Base_Image = new double[Base_Rows*Base_Columns];

	Base_Image = read_text(FileName, Base_Rows, Base_Columns);

	write_pgm(pgmFileName, Base_Image, Base_Rows, Base_Columns, 255);

	char *FileName2 = Wally;
	ifstream File2(FileName2);
	int Ref_Rows = 0;
	int Ref_Columns = 0;

	getline(File2, Line);
	Ref_Rows++;

	istringstream iss2(Line);

	while (iss2 >> word) {
		Ref_Columns++;
	}

	while (getline(File2, Line)) {
		Ref_Rows++;
	}

	double* Ref_Image = new double[Ref_Rows*Ref_Columns];

	Ref_Image = read_text(FileName2, Ref_Rows, Ref_Columns);

	write_pgm(pgmFileName2, Ref_Image, Ref_Rows, Ref_Columns, 255);

	
	int* point = NNS_Algorithm(Base_Image, Ref_Image, Base_Rows, Base_Columns, Ref_Rows, Ref_Columns);
	for (int x = 0; x < Ref_Columns; x++) {
		for (int y = 0; y < Ref_Rows; y++) {
			Base_Image[(point[0]+x) + (point[1]+y) * Base_Columns]=0;
		}
	}

	write_pgm(pgmWallyFound, Base_Image, Base_Rows, Base_Columns, 255); // Output pgm with wally found.
	cout << "File Output " << endl;
	//void write_pgm(char *filename, double *data, int sizeR, int sizeC, int Q)
	cout << "Program End" << endl;
	system("Pause");
	return 0;
}

int* NNS_Algorithm(double* Base_Image, double* Ref_Image, int Base_Rows, int Base_Columns, int Ref_Rows, int Ref_Columns)
{
	cout << "Making Similarity Matrix" << endl;
	//this is the similarity measure of each spot in the base image
	double** similarityMatrix = new double*[Base_Columns];
	for (int i = 0; i < Base_Columns; i++) {
		similarityMatrix[i] = new double[Base_Rows];
	}
	cout << "Getting Similarity" << endl;
	//get the similarity
	for (int i = 0; i < Base_Columns-Ref_Columns; i++) {
		for (int j = 0; j < Base_Rows-Ref_Rows; j++) { 
			double similarity = 0;
			int count=0;
			for (int x = 0; x < Ref_Columns; x++) {
				for (int y = 0; y < Ref_Rows; y++) {
					//cout << "Comparing: "<<i+x<<"," << j+y<< " to "<<x<<"," <<y << endl;
					if (Ref_Image[x + y * Ref_Columns] >= 255) {
						break;
					}
					count++;
					double pixel1= Base_Image[(i + x) + (j + y) * Base_Columns];
					double pixel2 = Ref_Image[x + y * Ref_Columns];
					similarity += (255 - abs(pixel1-pixel2))/255;
					/*
					cout << "Base Pixel: " << pixel1 << endl;
					cout << "Ref Pixel: " << pixel2 << endl;
					cout << "Difference: " << abs(pixel1 - pixel2) << endl;
					cout << "Similarity: " << similarity << endl;
					*/
					//Get difference between the base image pixel and the ref image pixel to find the similarity.
					//Closer to one the more similar they are.
					
				}
			}
			similarity /= count;
			//cout << "Similarity: \"" << similarity << "\"" << endl;
			similarityMatrix[i][j] = similarity;
		}

		if (i%50==0) { cout << "Row " << i << " done." << endl; }
	}

	cout << "Finding Best Similarity" << endl;
	//find the highest similarity
	double best = -1;
	int bestx = -1, besty = -1;
	for (int i = 0; i < Base_Columns - Ref_Columns; i++) {
		for (int j = 0; j < Base_Rows - Ref_Rows; j++) {
			if (best == -1) {//if its the first one, make it the best
				bestx = i;
				besty = j;
			}

			//otherwise compare to current best
			double comparisonSpot = similarityMatrix[i][j];
			if (comparisonSpot > best) {
				best = comparisonSpot;
				bestx = i;
				besty = j;
			}

		}
	}

	cout << "Best Similarity Pos: " << bestx << ", " << besty << endl;
	cout << "Best Similarity Value: " << best << endl;
	int* pos=new int[2];
	pos[0] = bestx;
	pos[1] = besty;
	delete similarityMatrix; //Memory Managment.
	return pos;
	//Returns the position.
}



// Reads .txt file representing an image of R rows and C Columns stored in filename 
// and converts it to a 1D array of doubles of size R*C
// Memory allocation is performed inside readTXT
// Read .txt file with image of size (R rows x C columns), and convert to an array of doubles
double* read_text(char *fileName, int sizeR, int sizeC)
{
	double* data = new double[sizeR*sizeC];
	int i = 0;
	ifstream myfile(fileName);
	if (myfile.is_open())
	{

		while (myfile.good())
		{
			if (i>sizeR*sizeC - 1) break;
			myfile >> *(data + i);
			//cout << *(data + i) << ' '; // This line display the converted data on the screen, you may comment it out. 
			i++;
		}
		cout << "File Opened" << endl;
		myfile.close();
	}

	else cout << "Unable to open file" << endl;;
	//cout << i;

	return data;
}


// Converts a 1D array of doubles of size R*C to .pgm image of R rows and C Columns 
// and stores .pgm in filename
// Use Q = 255 for greyscale images and 1 for binary images.
void write_pgm(char *filename, double *data, int sizeR, int sizeC, int Q)
{

	int i;
	unsigned char *image;
	ofstream myfile;

	image = (unsigned char *) new unsigned char[sizeR*sizeC];

	// convert the integer values to unsigned char

	for (i = 0; i<sizeR*sizeC; i++)
		image[i] = (unsigned char)data[i];

	myfile.open(filename, ios::out | ios::binary | ios::trunc);

	if (!myfile) {
		cout << "Can't open file: " << filename << endl;
		exit(1);
	}

	myfile << "P5" << endl;
	myfile << sizeC << " " << sizeR << endl;
	myfile << Q << endl;

	myfile.write(reinterpret_cast<char *>(image), (sizeR*sizeC) * sizeof(unsigned char));

	if (myfile.fail()) {
		cout << "Can't write image " << filename << endl;
		exit(0);
	}

	myfile.close();

	delete[] image;
}
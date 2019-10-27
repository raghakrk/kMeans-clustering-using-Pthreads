#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<iostream>
#include <omp.h>

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"
using namespace std;
float means[4] = { 0,85,170,255 };

int classify(int val)
{
	int min = INT8_MAX;
	int index = -1;
	float dist;
	for (int i = 0; i < 4; i++)
	{
		dist = sqrt(pow(val- means[i],2));
		if (dist < min)
		{
			min = dist;
			index = i;
		}
	}
return index;
}


int main(int argc, char** argv) {
	struct timespec start, stop; 
  	double time;	
	int i;
	FILE* fp;

	unsigned char* a = (unsigned char*)malloc(sizeof(unsigned char) * h * w);

	// the matrix is stored in a linear array in row major fashion
	if (!(fp = fopen(input_file, "rb"))) {
		cout << "can not opern file" << endl;
		return 1;
	}
	fread(a, sizeof(unsigned char), w * h, fp);
	fclose(fp);

	// measure the start time here
	if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}

	//  Your code goes here
	int iterations = 0; int val; int index; int clusterSize[4];
	unsigned char* b = (unsigned char*)calloc(h * w,sizeof(unsigned char));
	while (iterations<30)
	{
		for (i = 0; i < 800 * 800; i++)
		{
			val = double(a[i]);
			index = classify(val);
			b[i] = index;
		}
		for (int k = 0; k < 4; k++)
		{
			int temp = 0;
			int cnt = 0;
			for (i = 0; i < 800 * 800; i++)
			{
				if (b[i] == k)
				{
					b[i] = 64 * k + 63;
					temp += b[i];
					cnt += 1;
				}
			}
			means[k] = temp / cnt;
			clusterSize[k] = cnt;
		}


		iterations += 1;

	}
	// measure the end time here
	if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}		
	  time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

	// print out the execution time here
	cout<<"Number of FLOPs = " <<2*h*w<<", Execution time = "<<time<<" sec,"<<2*h*w/time/1e6<<" MFLOPs per sec"<<endl;

	if (!(fp = fopen(output_file, "wb"))) {
	cout << "can not opern file" << endl;
	return 1;
	}
	fwrite(b, sizeof(unsigned char), w * h, fp);
	fclose(fp);
	return 0;
}

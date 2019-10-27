#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include<iostream>

#define h  800
#define w  800

#define input_file  "input.raw"
#define output_file "output.raw"
unsigned char* a = (unsigned char*)malloc(sizeof(unsigned char) * h * w);
unsigned char* b = (unsigned char*)calloc(h * w,sizeof(unsigned char));

using namespace std;
float means[6] = { 0,65,100,125,190,255};
float csum[6]={0,0,0,0,0,0};
float cnt[6]={0,0,0,0,0,0};
int clustval[6]={0,51,102,153,204,255};

int classify(int val)
{
	int min = INT8_MAX;
	int index = -1;
	float dist;
	for (int i = 0; i < 6; i++)
	{
		dist = sqrt(pow(val- means[i],2));
		if (dist < min)
		{
			min = dist;
			index = i;
		}
		
	}
	csum[index]+=val;
	cnt[index]+=1;
	return index;
}


int main(int argc, char** argv) {
	struct timespec start, stop; 
  	double time;	
	int i;
	FILE* fp;

	

	// the matrix is stored in a linear array in row major fashion
	if (!(fp = fopen(input_file, "rb"))) {
		cout << "can not opern file" << endl;
		return 1;
	}
	fread(a, sizeof(unsigned char), w * h, fp);
	fclose(fp);

	// measure the start time here
	if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}

	// main code goes here
	int iterations = 0; int val; int index;

	while (iterations<50)
	{
		for (i = 0; i < 800 * 800; i++)
		{
			val = double(a[i]);
			index = classify(val);
			
		}
		for (int k = 0; k < 6; k++)
		{
			means[k] = csum[k] / cnt[k];
		}

		iterations += 1;

	}
	for (int i=0;i<800*800;i++)
	{
		float dist;
		int minv = INT8_MAX;
		int index=-1;
		for (int j=0;j<6;j++)
		{	dist = abs((a[i]- means[j]));
			if (dist < minv)
			{
				minv = dist;
				index = j;
			}
		}
		b[i]=clustval[index];
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

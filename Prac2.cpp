//==============================================================================
// Copyright (C) John-Philip Taylor
// tyljoh010@myuct.ac.za
//
// This file is part of the EEE4084F Course
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
//==============================================================================

#include "Prac2.h"
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <math.h>
//------------------------------------------------------------------------------

using namespace std;

void threaded_run(char * filename){

    pthread_mutex_init(&Mutex, 0);

    if(!Input.Read(filename)){
        cerr << "Failed to read in the image: " << filename << endl;
        exit(2);
    }

    // Allocate space for output image.
    if(!(Threaded_Output.Allocate(Input.Width, Input.Height, Input.Components))){
        cerr << "Failed to allocate space for output image" << filename << endl;
        exit(2);
    }

    
    // Keep track of the threads using IDs
    int Thread_ID[Thread_Count];

    // Array of PThreads
    pthread_t Thread[Thread_Count];

    // start measuring time
    tic();

    for(int i=0; i < Thread_Count; i++){
        Thread_ID[i] = i;
        pthread_create(Thread+i, 0, Thread_Main, Thread_ID+i);
    }

    // Print threads have been created
    pthread_mutex_lock(&Mutex);
    cout << "Threads have been created." << endl;
    pthread_mutex_unlock(&Mutex);

    for(int i =0; i < Thread_Count; i++){
        if(pthread_join(Thread[i], 0)){
            pthread_mutex_lock(&Mutex);
            cerr << "There was a problem accessing thread no.: " << i << endl;
        }
    }
    cout << "Time taken for threads to run = " << toc()/1e-3 << "ms" << endl;

    // Clean-up
    pthread_mutex_destroy(&Mutex);
}

// This is each thread's "main" function.  It receives a unique ID
void* Thread_Main(void* Parameter){
    int ID = *((int*)Parameter);

    int current;
    int arrayOfValues[81];
    int median;
    int share = Input.Height / Thread_Count;
    int height_lim;
    if(ID == (Thread_Count - 1)){
        height_lim = Input.Height;
    }else {
        height_lim = (ID + 1) * share;
    }
    for(int k =0; k < Input.Components; k++){
        for(int i = ID * share; i < height_lim; i++){
            for(int j=k; j < Input.Width * Input.Components; j = j + 3){
                
                current = 0;
                for(int s = i - 4; s< i + 5; s++){
                    for(int t = j - 4 * Input.Components; t < j + 4 * Input.Components + 1; t = t+3){
                        if(s < 0 || t < 0 || s >= Input.Height || t >= Input.Width * Input.Components){
                            arrayOfValues[current] = Input.Rows[i][j];
                        }else {
                            arrayOfValues[current] = Input.Rows[s][t];
                        }
                        current++;
                    }
                }
                quickSort(arrayOfValues, 0, 80);
                median = arrayOfValues[41];
                for(int s = i - 4; s< i + 5; s++){
                    for(int t = j - 4 * Input.Components; t < j + 4 * Input.Components + 1; t = t+3){
                        if(!(s < 0 || t < 0 || s >= Input.Height || t >= Input.Width * Input.Components)){
                            Threaded_Output.Rows[s][t] = median;
                        }
                    }
                }
            }
        }
    }
    return 0;
}
//------------------------------------------------------------------------------

int main(int argc, char** argv)
{

    // Run 5 sequential and threaded runs and compare
    char * images[] = {"Data/fly.jpg", "Data/greatwall.jpg",  "Data/small.jpg"};
    
    for(int i = 0; i < 3; i++){

        cout << "Running tests for image: " << images[i] << endl;
        // Sequential run
        cout << "Sequential run" << endl;
        for(int j = 0; j < 3; j++)
            sequential_run(images[i]);

        Thread_Count = 16;
        cout << "Number of threads: " << Thread_Count << endl;
        // Threaded runs
        for(int j =0; j < 3; j++){
            threaded_run(images[i]);
        }

        Thread_Count = 8;
        cout << "Number of threads: " << Thread_Count << endl;

        // Threaded runs
        for(int i =0; i < 3; i++){
            threaded_run(images[i]);
        }

        correlationOfOutputs();

        if(!(Sequential_Output.Write("Data/Sequential_Output.jpg"))){
            cerr << "Error writing image file";
            exit(-1);
        }

            // Write the output image
        if(!Threaded_Output.Write("Data/Threaded_Output.jpg")){
            cout << "Cannot write image" << endl;
            exit(-3);
        }


    }

}

/**
 * The basic sequential run of the program. Should time.
 */
void sequential_run(char * filename){
        
    // Read the image in.
    if(!Sequential_Input.Read(filename)){
        cerr << "Failed to read in the image: " << filename << endl;
        exit(2);
    }

    // Allocate space for output image.
    if(!(Sequential_Output.Allocate(Sequential_Input.Width, Sequential_Input.Height, Sequential_Input.Components))){
        cerr << "Failed to allocate space for output image" << filename << endl;
        exit(2);
    }

    tic();

    int arrayOfValues[81];
    int current =0;
    int median = 0;
    // Iterate through the components individually and find median.
    // Outer loop is for the components
    for(int k =0; k < Sequential_Input.Components; k++){
        for(int i = 0; i < Sequential_Input.Height; i++){
            for(int j=k; j < Sequential_Input.Width * Sequential_Input.Components; j = j + 3){
                
                current = 0;
                for(int s = i - 4; s< i + 5; s++){
                    for(int t = j - 4 * Sequential_Input.Components; t < j + 4 * Sequential_Input.Components + 1; t = t+3){
                        if(s < 0 || t < 0 || s >= Sequential_Input.Height || t >= Sequential_Input.Width * Sequential_Input.Components){
                            arrayOfValues[current] = Sequential_Input.Rows[i][j];
                        }else {
                            arrayOfValues[current] = Sequential_Input.Rows[s][t];
                        }
                        current++;
                    }
                }
                bubble_sort(arrayOfValues, 81);
                median = arrayOfValues[41];
                for(int s = i - 4; s< i + 5; s++){
                    for(int t = j - 4 * Sequential_Input.Components; t < j + 4 * Sequential_Input.Components + 1; t = t+3){
                        if(!(s < 0 || t < 0 || s >= Sequential_Input.Height || t >= Sequential_Input.Width * Sequential_Input.Components)){
                            Sequential_Output.Rows[s][t] = median;
                        }
                    }
                }
            }
        }
    }

    printf("Time for sequential run is %lg ms\n", (double)toc()/1e-3);

}


void bubble_sort(int array[], int length){
    bool swapped;
    int a;
    for(int i = 1; i < length; i++){
        swapped = false;
        for(int j =0; j < length - 1; j++){
            // if previous element is bigger than the next
            if(array[j] > array[j +1]){
                // swap the elements
                a = array[j];
                array[j] = array[j +1];
                array[j + 1] = a;
                // set swapped to to true
                swapped = true;
            }
        }
        if(!swapped){
            break;
        }
    }
}

void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 
  
/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition (int arr[], int low, int high) 
{ 
    int pivot = arr[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (arr[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
            swap(&arr[i], &arr[j]); 
        } 
    } 
    swap(&arr[i + 1], &arr[high]); 
    return (i + 1); 
} 
  
/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int arr[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(arr, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(arr, low, pi - 1); 
        quickSort(arr, pi + 1, high); 
    } 
}

/**
 * Calculates the correlation of the Threaded and Sequential outputs given 3 components.
 */
void correlationOfOutputs(void){

    int total = Input.Height * Input.Width;
    int threaded_red[total];
    int threaded_green[total];
    int threaded_blue[total];

    int seq_red[total];
    int seq_green[total];
    int seq_blue[total];
    
    int index = 0;
    for(int i =0; i < Input.Height; i++){
        for(int j= 0; j < Input.Width * Input.Components; j =j+3){
            // Assign threaded values
            threaded_red[index] = Threaded_Output.Rows[i][j];
            threaded_blue[index] = Threaded_Output.Rows[i][j+1];
            threaded_green[index] = Threaded_Output.Rows[i][j+2];

            seq_red[index] = Sequential_Output.Rows[i][j];
            seq_green[index] = Sequential_Output.Rows[i][j+1];
            seq_blue[index] = Sequential_Output.Rows[i][j+ 2];
            index++;
        }
    }

    cout << "Calculating correlations" << endl;
    float r_red =  correlations(seq_red, threaded_red, total);
    float r_green = correlations(seq_green, threaded_green, total);
    float r_blue = correlations(seq_blue,threaded_blue, total);
    
    cout << "The correlation for red: " << r_red << endl;
    cout << "The correlation for green: " << r_green << endl;
    cout << "The correlation for blue: " << r_blue << endl;
    cout << "The average correlation: " << (r_red + r_green + r_blue)/3 << endl;
}

/**
 * Calculates the correlation of values in two arrays given the arrays and their lengths
 */
float correlations(int array_x[], int array_y[], int length){
    double sum_x =0 , sum_y = 0, sum_xy =0, sum_x2 =0, sum_y2=0;
    float ave_x=0, ave_y=0;
    int array_xy[length];

    for (int i = 0; i < length; i++){
        sum_x += array_x[i];
        sum_x2 += array_x[i] * array_x[i];

        sum_y += array_y[i];
        sum_y2 += array_y[i] * array_y[i];

        sum_xy += array_x[i] * array_y[i];
    }

    ave_x = sum_x /length;
    ave_y = sum_y /length;

    float num = length * sum_xy - sum_x * sum_y;
    float dem = sqrt( (length * sum_x2) - (sum_x * sum_x)) * sqrt((length * sum_y2) - (sum_y * sum_y));

    float r = num / dem;
    return r;
}

//------------------------------------------------------------------------------

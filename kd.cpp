/*
	Author: Paul Couch
	Date: 3/11/2020
	Project: CS 315 Assignment 3
*/

#include <stdlib.h>
#include <vector>
#include <iostream>

using namespace std;

const int K = 4; //dimension of data points
const int B = 10; //max points in a bucket

typedef struct point_s {
	int w, x, y, z;
} point;

typedef struct node_s {
	int dimension;
	int discriminant;
	vector<point> bucket;
	node_s* right;
	node_s* left;
} node;

void printPoint(point p) {
	cout << "(" << p.w << ", " << p.x << ", " << p.y << ", "
		<< p.z << ")" << endl;
}

int findDiscriminant(int arr [][K], int numNodes) {
	int discriminant, low, high, range;
	int maxRange = 0;
	for (int i = 0; i < K; i++) {
		low = high = arr[0][i];
		for (int j = 1; j < numNodes; j++) {
			if (arr[j][i] < low) 
				low = arr[j][i];
			if (arr[j][i] > high) 
				high = arr[j][i];
		}//find high and low values for each row in matrix
		range = high - low;
		if (range > maxRange) {
			maxRange = range;
			discriminant = i;
		}//check if given row has highest range
	}
	return discriminant;
}//end find discriminant

int lomutoPartition(int arr[], int left, int right) {
	int temp;
	int pivotIndex = (left + right) / 2;  //pick leftmost value as pivot
	int pivot = arr[pivotIndex];
	temp = arr[right];
	arr[right] = arr[pivotIndex];    //move pivot to end of array
	arr[pivotIndex] = temp;
	pivotIndex = left;
	for (int i = left; i < right; i++) {
		if (arr[i] < pivot) {
			temp = arr[i];
			arr[i] = arr[pivotIndex];
			arr[pivotIndex] = temp;
			pivotIndex++;
		}//move elements less than the pivot in front of it
	}
	return pivotIndex;
}//end lomutoPartition

int findMedian(int arr[], int left, int right, int numNodes) {
	if (left == right) return arr[right]; //check if array is one number long
	int pivotIndex = lomutoPartition(arr, left, right);
	if (numNodes / 2 == pivotIndex)
		return arr[pivotIndex]; //check if median has been found, if not recurse
	else if (numNodes / 2 < pivotIndex)
		return findMedian(arr, left, pivotIndex - 1, numNodes);
	else 
		return findMedian(arr, pivotIndex + 1, right, numNodes);
	return 0;
}//end findMedian


node* buildTree(int arr[][K], int numNodes) {
	node* newNode = (node*)malloc(sizeof(node));
	if (numNodes > B) {
		int discriminant = findDiscriminant(arr, numNodes);//find dimension to discriminate on
		int column[numNodes];
		for (int i = 0; i < numNodes; i++) {
			column[i] = arr[i][discriminant];
		}
		int median = findMedian(column, 0, numNodes - 1, numNodes);//find median value in that dimension
		newNode->dimension = discriminant;
		newNode->discriminant = median;
		int smaller[numNodes / 2 + 1][K];
		int bigger[numNodes / 2 + 1][K];
		int smallIndex = 0;
		int bigIndex = 0;
		for (int i = 0; i < numNodes; i++) {
			if (arr[i][discriminant] <= median) {
				smaller[smallIndex][0] = arr[i][0];
				smaller[smallIndex][1] = arr[i][1];
				smaller[smallIndex][2] = arr[i][2];
				smaller[smallIndex][3] = arr[i][3];
				smallIndex++;
			}
			else {
				bigger[bigIndex][0] = arr[i][0];
				bigger[bigIndex][1] = arr[i][1];
				bigger[bigIndex][2] = arr[i][2];
				bigger[bigIndex][3] = arr[i][3];
				bigIndex++;
			}
		}//split array into two sub-arrays based on discriminant
		newNode->left = buildTree(smaller, smallIndex);
		newNode->right = buildTree(bigger, bigIndex);//build the subtrees by recursing
	}//check if sub-array is small enough to be a leaf
	else {
		for (int i = 0; i < numNodes; i++) {
			point* newPoint = (point*)malloc(sizeof(point));
			newPoint->w = arr[i][0];
			newPoint->x = arr[i][1];
			newPoint->y = arr[i][2];
			newPoint->z = arr[i][3];
			newNode->bucket.push_back(*newPoint);
		}//add points to the buckets of the leaves of the tree
	}
	return newNode;
}//end buildTree

void probeTree(int probe[K], node* node) {
	if (node->bucket.empty()) {//check if node is internal or leaf
		if (probe[node->dimension] <= node->discriminant) 
			probeTree(probe, node->left);
		else 
			probeTree(probe, node->right); //check the probe against the discriminant 
											//and dive down subtree
	}
	else {
		cout << "Probe: (" << probe[0] << ", " << probe[1] << ", " 
			<< probe[2] << ", " << probe[3] << ")" << endl;
		cout << "Contents of bucket: " << endl;
		for (int i = 0; i < (int)node->bucket.size(); i++) {
			printPoint(node->bucket[i]);
		}
	}
}

int main(int argc, char** argv) {
	int numNodes, numProbes;
	if (argc < 3) {
		cout << "Insufficient arguments." << endl;
		return 0;
	}// check if the number of arguments is correct
	else{
		numNodes = stoi(argv[1]);
		numProbes = stoi(argv[2]);
	}
	int points[numNodes][K];
	for (int i = 0; i < numNodes; i++) {
		for (int j = 0; j < K; j++) {
			cin >> points[i][j];
		}
	}//accumulate the points into an array
	node* root = buildTree(points, numNodes);
	int probe[K];
	for (int i = 0; i < numProbes; i++) {
		cin >> probe[0];
		cin >> probe[1];
		cin >> probe[2];
		cin >> probe[3];
		probeTree(probe, root);
	}
	return 0;
}// end main
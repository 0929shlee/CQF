//
// Created by Seunghyun Lee on 2021/11/14.
//

#ifndef CQF_CORE_H
#define CQF_CORE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


typedef unsigned int uint;
typedef vector<vector<vector<uint>>> Vector3D;
typedef vector<vector<uint>> Vector2D;
typedef vector<uint> Vector1D;
#define NEW_VECTOR3D_0 Vector3D(NUM_GNB, Vector2D(NUM_UE, Vector1D(NUM_TIME, 0)))

const string cqiMatrixFilePath = "cqi_matrix.txt";

const uint MAX_CQI_VAL = 15;
const uint MAX_GNB_CONNECT = 2/*64*/;

const uint NUM_GNB = 5;
const uint NUM_UE = 10/*100*/;
const uint NUM_TIME = 100;

#endif //CQF_CORE_H
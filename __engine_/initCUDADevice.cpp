#include "initCUDADevice.cuh"
#include <string>

bool initCUDADevice(const char* const deviceName) {

	cudaDeviceProp deviceProp;
	int deviceNumber = 0;
	int deviceId = -1;
	cudaGetDeviceCount(&deviceNumber);
	int counter = 0;
	bool deviceIsFinded = false;

	while (deviceId == -1 && counter < deviceNumber) {

		cudaGetDeviceProperties(&deviceProp, counter);

		if (!strcmp(deviceProp.name, deviceName)) {

			cudaChooseDevice(&deviceId, &deviceProp);
			cudaSetDevice(deviceId);
			deviceIsFinded = true;

		}

		counter++;

	}

	return deviceIsFinded;

}
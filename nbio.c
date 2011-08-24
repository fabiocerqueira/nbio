#include "Python.h"
#include "include/NBioAPI.h"

NBioAPI_DEVICE_ID m_DeviceID;
NBioAPI_DEVICE_ID m_OpenedDeviceID;
NBioAPI_DEVICE_INFO_0 m_DeviceInfo0;

// Handle for NBioBSP
NBioAPI_HANDLE m_hBSP;
NBioAPI_FIR_HANDLE m_hFIR;
NBioAPI_FIR m_FullFIR;
NBioAPI_FIR_TEXTENCODE m_TextFIR;

// Version
NBioAPI_VERSION m_Version;

long m_DefaultTimeout;
unsigned int m_ImageQuality;
unsigned int m_SecurityLevel;
unsigned int m_DeviceList;

void initnbio(void);

int main(int argc, char **argv) {
	Py_Initialize();
	initnbio();
	Py_Exit(0);
	return 0;
}


static PyObject *nbio_get_info(PyObject *self, PyObject* args)
{
	NBioAPI_INIT_INFO_0 initInfo0;
	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// Failed to init NBioBSP Module.
		return Py_None;
	}
	memset(&initInfo0, 0, sizeof(NBioAPI_INIT_INFO_0));
	NBioAPI_RETURN err = NBioAPI_GetInitInfo(m_hBSP, 0, &initInfo0);
	if (err == NBioAPIERROR_NONE) {
		// Function success - [Get Info]
		m_ImageQuality = initInfo0.VerifyImageQuality;
		m_DefaultTimeout = initInfo0.DefaultTimeout;
		m_SecurityLevel = initInfo0.SecurityLevel;
		return Py_True;
	} 
	else {
		printf("ERROR: %d", err);
		return Py_False;
	}
}


static PyMethodDef nbio_methods[] = {
	{"get_info",nbio_get_info,METH_NOARGS, "NBio get_info"},
	{NULL, NULL} //sentinela
};


void initnbio(void)
{
	PyImport_AddModule("nbio");
	Py_InitModule("nbio", nbio_methods);
}

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

unsigned char* m_pEnrollBuffer;
unsigned char* m_pVerifyBuffer;


void initnbio(void);

int main(int argc, char **argv) {
	Py_Initialize();
	initnbio();
	Py_Exit(0);
	return 0;
}

static PyObject *nbio_init(PyObject *self, PyObject* args)
{
	NBioAPI_RETURN err;

	m_hBSP = NBioAPI_INVALID_HANDLE;
	m_hFIR = NBioAPI_INVALID_HANDLE;
	memset(&m_FullFIR, 0, sizeof(NBioAPI_FIR));
	memset(&m_TextFIR, 0, sizeof(NBioAPI_FIR_TEXTENCODE));

	m_pEnrollBuffer = NULL;
	m_pVerifyBuffer = NULL;

	err = NBioAPI_Init(&m_hBSP);
	if (err == NBioAPIERROR_NONE) {
		NBioAPI_GetVersion(m_hBSP, &m_Version);
		// NBioBSP Module Init success
		return Py_True;
	}
	else {
		printf("ERROR %d", err);
		return Py_False;
	}

}

static PyObject *nbio_close(PyObject *self, PyObject* args)
{
	if (m_pEnrollBuffer)
		free(m_pEnrollBuffer);
	if (m_pVerifyBuffer)
		free(m_pVerifyBuffer);

	// Terminate NBioBSP module
	if(m_hBSP != NBioAPI_INVALID_HANDLE) {
		NBioAPI_FreeFIR(m_hBSP, &m_FullFIR);
		NBioAPI_FreeTextFIR(m_hBSP, &m_TextFIR);
		NBioAPI_FreeFIRHandle(m_hBSP, m_hFIR);
		NBioAPI_CloseDevice(m_hBSP, m_DeviceID);
		NBioAPI_Terminate(m_hBSP);
	}
	return Py_None;
}

static PyObject *nbio_get_info(PyObject *self, PyObject* args)
{
	NBioAPI_INIT_INFO_0 initInfo0;
	NBioAPI_RETURN err;

	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// Failed to init NBioBSP Module.
		return Py_None;
	}
	memset(&initInfo0, 0, sizeof(NBioAPI_INIT_INFO_0));
	err = NBioAPI_GetInitInfo(m_hBSP, 0, &initInfo0);
	if (err == NBioAPIERROR_NONE) {
		// Function success - [Get Info]
		m_ImageQuality = initInfo0.VerifyImageQuality;
		m_DefaultTimeout = initInfo0.DefaultTimeout;
		m_SecurityLevel = initInfo0.SecurityLevel;
		return Py_BuildValue("{s:i,s:i,s:i}",
              		"image_quality", m_ImageQuality, 
			"default_timeout", m_DefaultTimeout,
			"security_level", m_SecurityLevel
			); 
	} 
	else {
		printf("ERROR: %d", err);
		return Py_False;
	}
}

static PyObject *nbio_set_info(PyObject *self, PyObject* args)
{
	int val_VIQ, val_DT;
	NBioAPI_RETURN err;

	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// "Failed to init NBioBSP Module."
		return Py_None;
	}

	if (!PyArg_ParseTuple(args, "ll:addi", &val_VIQ, &val_DT)) {
		return NULL;
	}

	m_ImageQuality = val_VIQ;
	m_DefaultTimeout = val_DT;

	if (m_ImageQuality <= 0 || m_ImageQuality > 100 || m_DefaultTimeout < 0) {
		//"Function failed - [Set Info : Invalid param]"
		return Py_False;
	} 

	NBioAPI_INIT_INFO_0 initInfo0;
	memset(&initInfo0, 0, sizeof(initInfo0));

	err  = NBioAPI_GetInitInfo(m_hBSP,0, &initInfo0);
	if (err == NBioAPIERROR_NONE) {
		initInfo0.StructureType = 0;
		initInfo0.VerifyImageQuality = m_ImageQuality;
		initInfo0.DefaultTimeout = m_DefaultTimeout;
		initInfo0.SecurityLevel = m_SecurityLevel;

		err = NBioAPI_SetInitInfo(m_hBSP,0, &initInfo0);
	}
	if (err == NBioAPIERROR_NONE) {
		// "Function success - [Set Info]"
		return Py_True;
	}
	else {
		printf("ERROR: %d", err);
		return Py_False;
	}
}

static PyMethodDef nbio_methods[] = {
	{"init",nbio_init,METH_NOARGS, "NBio init"},
	{"close",nbio_close,METH_NOARGS, "NBio init"},
	{"get_info",nbio_get_info,METH_NOARGS, "NBio get_info"},
	{"set_info",nbio_set_info,METH_VARARGS, "Nbio set_info"},
	{NULL, NULL} //sentinela
};


void initnbio(void)
{
	PyImport_AddModule("nbio");
	Py_InitModule("nbio", nbio_methods);
}

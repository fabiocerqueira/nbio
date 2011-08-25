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
static PyObject *display_error(NBioAPI_RETURN errCode);

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
		return display_error(err);
	}

}

static PyObject *nbio_open(PyObject *self, PyObject* args)
{
	NBioAPI_RETURN err;
	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// Failed to init NBioBSP Module.
		return Py_None;
	}

	NBioAPI_CloseDevice(m_hBSP, m_OpenedDeviceID);
	err = NBioAPI_OpenDevice(m_hBSP, m_DeviceID);
	if (err == NBioAPIERROR_DEVICE_ALREADY_OPENED) {
		NBioAPI_CloseDevice(m_hBSP, m_DeviceID);
		err = NBioAPI_OpenDevice(m_hBSP, m_DeviceID);
	}

	if (err == NBioAPIERROR_NONE) {
		memset(&m_DeviceInfo0, 0, sizeof(NBioAPI_DEVICE_INFO_0));
		m_DeviceInfo0.StructureType = 0;
		NBioAPI_GetDeviceInfo(m_hBSP, NBioAPI_DEVICE_ID_AUTO, 0, &m_DeviceInfo0);

		if (m_pEnrollBuffer)
			free(m_pEnrollBuffer);
		if (m_pVerifyBuffer)
			free(m_pVerifyBuffer);

		// "Function success - [Open Device]"
		return Py_True;
	}
	else {
		return display_error(err);
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
		return display_error(err);
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
		return display_error(err);
	}
}

static PyObject *display_error(NBioAPI_RETURN errCode)
{
	PyObject *error_msg;
	char *error_generic = "0x0000";
	switch (errCode) {
		case 0x0000:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_NONE");
			break;
		case 0x0100:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_BASE_DEVICE");
			break;
		case 0x0200:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_BASE_UI");
			break;

		case 0x0001:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INVALID_HANDLE");
			break;
		case 0x0002:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INVALID_POINTER");
			break;
		case 0x0003:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INVALID_TYPE");
			break;
		case 0x0004:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_FUNCTION_FAIL");
			break;
		case 0x0005:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_STRUCTTYPE_NOT_MATCHED");
			break;
		case 0x0006:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_ALREADY_PROCESSED");
			break;
		case 0x0007:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_EXTRACTION_OPEN_FAIL");
			break;
		case 0x0008:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_VERIFICATION_OPEN_FAIL");
			break;
		case 0x0009:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DATA_PROCESS_FAIL");
			break;
		case 0x000a:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_MUSE_BE_PROCESSED_DATA");
			break;
		case 0x000b:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INTERNAL_CHECKSUM_FAIL");
			break;
		case 0x000c:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_ENCRYPTED_DATA_ERROR");
			break;
		case 0x000d:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_UNKNOWN_FORMAT");
			break;
		case 0x000e:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_UNKNOWN_VERSION");
			break;
		case 0x000f:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_VALIDITY_FAIL");
			break;

		case 0x0010:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_MAXFINGER");
			break;
		case 0x0011:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_SAMPLESPERFINGER");
			break;
		case 0x0012:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_ENROLLQUALITY");
			break;
		case 0x0013:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_VERIFYQUALITY");
			break;
		case 0x0014:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_IDENTIFYQUALITY");
			break;
		case 0x0015:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INIT_SECURITYLEVEL");
			break;

		case 0x0101:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_OPEN_FAIL");
			break;
		case 0x0102:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_INVALID_DEVICE_ID");
			break;
		case 0x0103:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_WRONG_DEVICE_ID");
			break;
		case 0x0104:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_ALREADY_OPENED");
			break;
		case 0x0105:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_NOT_OPENED");
			break;
		case 0x0106:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_BRIGHTNESS");
			break;
		case 0x0107:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_CONTRAST");
			break;
		case 0x0108:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_DEVICE_GAIN");
			break;

		case 0x0201:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_USER_CANCEL");
			break;
		case 0x0202:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_USER_BACK");
			break;
		case 0x0203:
			error_msg = Py_BuildValue("s", "NBioAPIERROR_CAPTURE_TIMEOUT");
			break;
		default:
			sprintf(error_generic, "0x%04x", errCode);
			error_msg = Py_BuildValue("s", error_generic);
			break;
	}
	printf(PyString_AsString(error_msg));
	return Py_False;
}

static PyMethodDef nbio_methods[] = {
	{"init",nbio_init,METH_NOARGS, "NBio init"},
	{"open",nbio_open,METH_NOARGS, "NBio open"},
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

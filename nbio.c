#include "Python.h"
#include "include/NBioAPI.h"

NBioAPI_DEVICE_ID m_DeviceID;
NBioAPI_DEVICE_INFO_0 m_DeviceInfo0;

// Handle for NBioBSP
NBioAPI_HANDLE m_hBSP;
NBioAPI_FIR_HANDLE m_hFIR;
// Version
NBioAPI_VERSION m_Version;

// Info
long m_DefaultTimeout;
unsigned int m_ImageQuality;
unsigned int m_SecurityLevel;
unsigned int m_DeviceList;

// Data fingerprint
NBioAPI_FIR_TEXTENCODE m_TextFIR;

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
	memset(&m_TextFIR, 0, sizeof(NBioAPI_FIR_TEXTENCODE));

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

	NBioAPI_CloseDevice(m_hBSP, m_DeviceID);
	m_DeviceID = NBioAPI_DEVICE_ID_AUTO;
	err = NBioAPI_OpenDevice(m_hBSP, m_DeviceID);
	if (err == NBioAPIERROR_DEVICE_ALREADY_OPENED) {
		NBioAPI_CloseDevice(m_hBSP, m_DeviceID);
		err = NBioAPI_OpenDevice(m_hBSP, m_DeviceID);
	}

	if (err == NBioAPIERROR_NONE) {
		memset(&m_DeviceInfo0, 0, sizeof(NBioAPI_DEVICE_INFO_0));
		m_DeviceInfo0.StructureType = 0;
		NBioAPI_GetDeviceInfo(m_hBSP, NBioAPI_DEVICE_ID_AUTO, 0, &m_DeviceInfo0);
		// "Function success - [Open Device]"
		return Py_True;
	}
	else {
		return display_error(err);
	}
}

static PyObject *nbio_close(PyObject *self, PyObject* args)
{

	// Terminate NBioBSP module
	if(m_hBSP != NBioAPI_INVALID_HANDLE) {
		NBioAPI_FreeTextFIR(m_hBSP, &m_TextFIR);
		NBioAPI_FreeFIRHandle(m_hBSP, m_hFIR);
		NBioAPI_CloseDevice(m_hBSP, m_DeviceID);
		NBioAPI_Terminate(m_hBSP);
		return Py_True;
	}
	return Py_False;
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
		if (err == NBioAPIERROR_NONE) {
			return Py_True;
		}
	}
	return display_error(err);
}
static PyObject *nbio_enroll(PyObject *self, PyObject* args)
{
	NBioAPI_RETURN err;
	NBioAPI_FIR_PAYLOAD payload;
	NBioAPI_BOOL bResult = NBioAPI_FALSE;
	NBioAPI_FIR_HANDLE hCapturedFIR1 = NBioAPI_INVALID_HANDLE;
	NBioAPI_FIR_HANDLE hCapturedFIR2 = NBioAPI_INVALID_HANDLE;
	NBioAPI_INPUT_FIR inputCapture1;
	NBioAPI_INPUT_FIR inputCapture2;

	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// Failed to init NBioBSP Module.
		return Py_None;
	}

	char *chUserData;
	if (!PyArg_ParseTuple(args, "s:addi", &chUserData)) {
		return NULL;
	}
	int nUserDataLen = strlen(chUserData);

	if (m_hFIR != NBioAPI_INVALID_HANDLE) {
		NBioAPI_FreeFIRHandle(m_hBSP, m_hFIR);
		m_hFIR = NBioAPI_INVALID_HANDLE;
	}

	err = NBioAPI_Capture(m_hBSP, NBioAPI_FIR_PURPOSE_VERIFY, &hCapturedFIR1, -1, NULL, NULL);
	if (err == NBioAPIERROR_NONE) {
		err = NBioAPI_Capture(m_hBSP, NBioAPI_FIR_PURPOSE_VERIFY, &hCapturedFIR2, -1, NULL, NULL);

		if (err == NBioAPIERROR_NONE) {
			inputCapture1.Form = NBioAPI_FIR_FORM_HANDLE;
			inputCapture1.InputFIR.FIRinBSP = &hCapturedFIR1;
			inputCapture2.Form = NBioAPI_FIR_FORM_HANDLE;
			inputCapture2.InputFIR.FIRinBSP = &hCapturedFIR2;

			err = NBioAPI_VerifyMatch(m_hBSP, &inputCapture1, &inputCapture2, &bResult, NULL);
			if (err == NBioAPIERROR_NONE) { // Verify Success
				if (bResult) {
					if (strlen(chUserData) > 0) {
						memset(&payload, 0, sizeof(NBioAPI_FIR_PAYLOAD));

						payload.Length = nUserDataLen;
						payload.Data = (NBioAPI_UINT8 *) malloc (payload.Length);
						memcpy(payload.Data, chUserData, payload.Length);

						err =  NBioAPI_CreateTemplate(m_hBSP, &inputCapture1, NULL, &m_hFIR, &payload);

						if (payload.Data)
							free(payload.Data);

					} 
					else {
						err =  NBioAPI_CreateTemplate(m_hBSP, &inputCapture1, NULL, &m_hFIR, NULL);
					}

					if (err == NBioAPIERROR_NONE) {
						// Get text fir from handle.
						NBioAPI_FreeTextFIR(m_hBSP, &m_TextFIR);
						NBioAPI_GetTextFIRFromHandle(m_hBSP, m_hFIR, &m_TextFIR, 0);
						/// "Enroll success"
						return Py_BuildValue("s", m_TextFIR.TextFIR);

					}
				}
				else {
					return Py_False;
				}
			}
		}
	}
	// Free memory
	NBioAPI_FreeFIRHandle(m_hBSP, hCapturedFIR1);
	NBioAPI_FreeFIRHandle(m_hBSP, hCapturedFIR2);


	if (err != NBioAPIERROR_NONE) {
		return display_error(err);
	}
	return Py_None;
}

static PyObject *nbio_verify(PyObject *self, PyObject* args)
{
	PyObject *ret;
	if (m_hBSP == NBioAPI_INVALID_HANDLE) {
		// "Failed to init NBioBSP Module."
		return Py_None;
	}

	if (m_hFIR == NBioAPI_INVALID_HANDLE) {
		// "Can not find enrolled fingerprint!"
		return Py_None;
	}

	NBioAPI_RETURN err;
	NBioAPI_INPUT_FIR storedFIR;
	NBioAPI_INPUT_FIR verifyFIR;

	storedFIR.Form = NBioAPI_FIR_FORM_TEXTENCODE;
	storedFIR.InputFIR.FIR = &m_TextFIR;

	NBioAPI_BOOL bResult = NBioAPI_FALSE;
	NBioAPI_FIR_HANDLE hVerifyFIR = NBioAPI_INVALID_HANDLE;
	NBioAPI_FIR_PAYLOAD payload;
	memset(&payload, 0, sizeof(NBioAPI_FIR_PAYLOAD));


	err = NBioAPI_Capture(m_hBSP, NBioAPI_FIR_PURPOSE_VERIFY, &hVerifyFIR, -1, NULL, NULL);
	if (err == NBioAPIERROR_NONE) {
		verifyFIR.Form = NBioAPI_FIR_FORM_HANDLE;
		verifyFIR.InputFIR.FIRinBSP = &hVerifyFIR;

		err = NBioAPI_VerifyMatch(m_hBSP, &verifyFIR, &storedFIR, &bResult, &payload);
	}

	if (err == NBioAPIERROR_NONE) {
		if (bResult) { // Verify Success
			if (payload.Length) {
				printf("Verify success! (User : %s)", payload.Data);
			}
			ret = Py_True;
		}
		else { // Verify failed
			ret = Py_False;
		}
	}
	else {
		ret = display_error(err);
	}

	// Free memory
	NBioAPI_FreePayload(m_hBSP, &payload);
	NBioAPI_FreeFIRHandle(m_hBSP, hVerifyFIR);
	return ret;
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
	{"enroll",nbio_enroll,METH_VARARGS, "NBio enroll"},
	{"verify",nbio_verify,METH_NOARGS, "NBio verify"},
	{NULL, NULL} //sentinela
};


void initnbio(void)
{
	PyImport_AddModule("nbio");
	Py_InitModule("nbio", nbio_methods);
}

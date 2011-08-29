import nbio
print 'INIT:', nbio.init()
print 'OPEN:', nbio.open()
print 'GET_INFO:', nbio.get_info()
print 'SET_INFO:', nbio.set_info(50,5000)
print 'GET_INFO:', nbio.get_info()
text = nbio.enroll()
print 'ENROLL:', text
print 'VERIFY:', nbio.verify(text)
print 'ENROLL TEXT1'
text1 = nbio.enroll()
print 'ENROLL TEXT2'
text2 = nbio.enroll()
print 'VERIFY_MATCH:', nbio.verify_match(text1, text2)
print 'CLOSE:', nbio.close()

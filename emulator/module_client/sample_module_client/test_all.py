import subprocess, time


print("=======================================================")
print("=======================================================")
print("Square signal:")
subprocess.run(['python', 'test_square_signal.py'])
print("Square signal test passed")
time.sleep(1)

print("=======================================================")
print("=======================================================")
subprocess.run(['python', 'testSC.py'])
print("Short circuit test passed")
time.sleep(1)

print("=======================================================")
print("=======================================================")
subprocess.run(['python', 'test_WB.py'])
print("Wirebreak test passed")
time.sleep(1)


print("=======================================================")
print("=======================================================")
subprocess.run(['python', 'test_CNT_4.py'])
print("test_CNT_4.py test passed")

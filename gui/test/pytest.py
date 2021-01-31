from subprocess import Popen, PIPE
import select
import time


x = Popen('/Users/zachbortoff/Projects/Cloak/cmake-build-debug/cloak', bufsize=0, stdin=PIPE, stdout=PIPE)
y = select.poll()
y.register(x.stdout,select.POLLIN)

# while True:
#     if y.poll(1):
#         print(x.stdout.readline())
#         break
#     else:
#         print("nothing here")
#         time.sleep(1)
print("writing...")
x.stdin.write("uci\n".encode())
buff_str = []
while True:
    if y.poll(1):
        s = x.stdout.readline().decode()
        print(s)
        buff_str.append(s)
        if s == 'uciok\n':
            break
    else:
        time.sleep(1)
print("writing...")
x.stdin.write("quit\n".encode())

x.wait(5)
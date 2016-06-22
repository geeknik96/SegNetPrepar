from PIL import Image 
import argparse
import random
import struct
import socket
import time
import os

class SegentServer:
    """
    server for segnetation images
    """
    MAX_CONNECTION = 100
    TARGET_IMAGES = 50
    IMG_SIZE = (320, 240)
    REQUEST_TYPE_SIZE = 4
    ID_USER_SIZE = 4
    NEW_CLIENT = -1

    GETREQ = 'GET@'
    ANSREQ = 'ANS@'
    BADREQ = 'BAD@'
    DONERESP = '->-@'


    def __init__(self, address, port, images_path, result_path='result', timeout=600.0):
        self._sock = socket.socket()
        self._sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self._sock.bind((str(address), int(port)))
        self._sock.listen(self.MAX_CONNECTION)
        self._result_path = result_path
        self._paths = set(images_path)
        self._timeout = timeout
        self._users = {}

        # print 'all task =', '\n'.join(list(self._paths))

        if not os.path.exists(result_path):
            os.mkdir(result_path)

        print 'server listen on', address, port

    def run(self):
        status = True, 'cleaning ids'
        while True:
            try:
                client, addr = self._sock.accept()
                read_size = self.REQUEST_TYPE_SIZE + self.ID_USER_SIZE
                data = client.recv(read_size)

                if not data:
                    status = False, 'got empty data'
                request_type, user_id = struct.unpack('4si', data)

                status = False, 'unknown error request = {0} {1}'.\
                    format(request_type, user_id)

                if request_type == self.GETREQ:
                    status = self.__get_request_processor(client, user_id)
                elif request_type == self.ANSREQ:
                    status = self.__ans_request_processor(client, user_id)
                else:
                    status = False, 'got thrash'

            finally:
                cur_time = time.asctime(time.localtime(time.time()))
                if status[0]:
                    print cur_time, addr, 'LOG:', status[1]
                else:
                    print cur_time, addr, 'ERROR:', status[1]
                client.close()
                res = self.__remove_on_timeout(self._timeout)
                if res[1]:
                    print cur_time, 'LOG:', 'remove(timeout) ids:', res[1]

    def __get_request_processor(self, client, user_id):
        try:
            if user_id == self.NEW_CLIENT:
                new_id = self.__generate_user_id()
                client.send(struct.pack('!i', new_id))
                return True, 'GET add new client with id {0} -> {1}'.format(user_id, new_id)
            self.__make_task_to_user(user_id)
            task = self._users[user_id][0]
            if task is None:
                client.send(struct.pack('!4s', self.DONERESP))
                return True, 'GET client with id {0} done'.format(user_id)
            img_data = self.__read_image(task)
            client.send(img_data)
            return True, 'GET send img client with id {0}'.format(user_id)
        except Exception as e:
            return False, 'GET error with client id {0} with error: {1}'.format(user_id, e)


    def __ans_request_processor(self, client, user_id):
        try:
            bytes_img = self.__recv(client)
            if not bytes_img:
                return False, 'ANS image empty for client id {0}'.format(user_id)
            self.__save_image(bytes_img, self._users[user_id][0], self._result_path)
            self._users[user_id][1] = time.time()
            self._users[user_id][2] += 1
            progress = (self._users[user_id][2])
            return True, 'ANS send image client with id {0} progress {1}'.format(user_id, progress)
        except Exception as e:
            return False, 'ANS error with client id {0} with error: {1}'.format(user_id, e)

    def __generate_user_id(self):
        for ID in xrange(10000):
            if ID not in self._users:
                return ID
        raise Exception('limit ID is exceeded')

    def __make_task_to_user(self, user_id):
        task = None
        if self._paths:
            task = random.choice(list(self._paths))
            self._paths.remove(task)
        self._users[user_id] = [task, time.time(), 0]

    def __remove_on_timeout(self, timeout):
        rem_id = list()
        for k, v in self._users.items():
            if time.time() - v[1] > float(timeout):
                rem_id.append(k)
                self._paths |= set(v[0])
                self._users.pop(k)
        return True, rem_id

    @staticmethod
    def __recv(socket):
        allData = ''
        while True:

            data = socket.recv(4096)
            if not data:
                socket.send('sps')
                socket.close()
                return allData
            allData += data


    @staticmethod
    def __save_image(data, path, save_dir):
        img = Image.frombytes('L', SegentServer.IMG_SIZE, data)
        namefile = os.path.split(path)[-1]
        savepath = os.path.join(save_dir, namefile) + '.png'
        img.save(savepath)

    @staticmethod
    def __read_image(path):
        img = Image.open(path)
        return img.tobytes()

if __name__ == '__main__':
    argparser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)

    argparser.add_argument('-f', '--file_paths', help='file with path for images', type=str, required=True)
    argparser.add_argument('-a', '--address', help='ip address server', type=str, default='localhost')
    argparser.add_argument('-p', '--port', help='listen port', type=int, default=31415)
    argparser.add_argument('-d', '--dir', help='dir for store result', type=str, default='result')
    argparser.add_argument('-t', '--timeout', help='delta time between client ANS request', type=float, default=600.0)

    args = argparser.parse_args()

    with open(args.file_paths) as filepaths:
        paths = [fn.strip() for fn in filepaths.readlines()]

    server = SegentServer(args.address, args.port, paths, args.dir, args.timeout)
    server.run()

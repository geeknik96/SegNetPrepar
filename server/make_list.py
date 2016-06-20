import glob
import sys
import os

glob_path = os.path.join(sys.argv[1], '*.png')

list_ = glob.glob(glob_path)

with open('result.txt', 'w') as file_:
        for line in list_:
                abs_path = os.path.abspath(line)
                file_.write(abs_path + '\n')
"""
The animation process:
	- Make an animation in Krita and select File>Render_Animation and export as png image sequence to the 'frames' directory
	- Run this script with two parameters as follows: python make-animation.py <name_of_animation> <length_in_seconds>
	- It will output two files to the 'assets' directory
		- One is a txt file containing the following:
			- On the top line is the length of the animation in seconds and the width of a frame in pixels
			- On each subsequent line is the duration of each frame as a portion of the total
		- The other file is a png of the spritesheet
"""
import os
import sys

framedir = 'frames'
outputdir = 'assets'
files = os.listdir(framedir)
files.sort()

# get width for single frame
os.system('magick identify '+framedir+'/'+files[len(files)-1]+' > tmp')
file_info = open('tmp').read()
os.remove('tmp')
width = file_info.split(' ')[2].split('x')[0]

# count and remove duplicate frames
total_frames = len(files)
frame_in_anim_index = -1
frame_counts = [0] * total_frames
for i in range(total_frames):
	if(i < total_frames - 1 and not os.system('cmp '+framedir+'/'+files[i]+' '+framedir+'/'+files[i+1])):
		print('duplicate frame')
		os.remove(framedir+'/'+files[i])
	else:
		frame_in_anim_index = i
	frame_counts[frame_in_anim_index] += 1

# convert frame counts to portions & write to output txt file
output = open(framedir+'/'+sys.argv[1]+'.txt', 'a')
output.write(sys.argv[2]+','+width+'\n')
remaining_frame_names = ''
for i in range(total_frames):
	if(frame_counts[i]):
		output.write((str)((float)(frame_counts[i]) / (float)(total_frames))+'\n')
		remaining_frame_names += framedir+'/'+files[i]+' '

# combine frames into single image & clean up
os.system('convert '+remaining_frame_names+'+append '+framedir+'/'+sys.argv[1]+'.png')
os.system('rm '+remaining_frame_names)
os.system('mv '+framedir+'/* '+outputdir)
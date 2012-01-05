import ossaudiodev
import time

class mic:
    def __init__(self):
        self.setup_sound()

    def setup_sound(self):
        self.timing_diff = 1000
        self.ossspeed = 100000
        self.osschannels = 1
        self.ossfmt = ossaudiodev.AFMT_S16_LE
        self.CORBA_being_used = False

        try:
            self.sound_driver = ossaudiodev.open('/dev/dsp','rw')
        except IOError:
            print "[test_mic] Failed to open /dev/dsp for read/write"
        self.osschannels = self.sound_driver.channels(2)
        #ossfmt = sound_driver.setfmt(ossaudiodev.AFMT_U8)
        self.ossfmt = self.sound_driver.setfmt(ossaudiodev.AFMT_S16_LE)
        self.ossspeed = self.sound_driver.speed(50000)
        self.sound_driver.nonblock()

    def run_loop(self):
        buffer = []
        my_len = 100000
        print "get"
        for x in range(my_len):
            try:
                data = self.sound_driver.read(1000)
                buffer.append(data)
            except:
                pass
        print "Waiting"
        time.sleep(2)
        print "done waiting"
        #print buffer
        while len(buffer) > 1:
            self.sound_driver.writeall(buffer.pop())

my_mic = mic()
my_mic.run_loop()





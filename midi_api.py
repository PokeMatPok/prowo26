import mido
from mido import MidiFile, MidiTrack, Message, MetaMessage
import time

def Is_valid_note(msg, MIDI_CHANNEL=0, NOTE_RANGE=(48, 84)):
    """Check if message is a valid note in our range and channel"""
    return (msg.type in ['note_on', 'note_off'] and 
            msg.channel == MIDI_CHANNEL and 
            NOTE_RANGE[0] <= msg.note <= NOTE_RANGE[1])

def ToNoteValue(note_str: str):
    # get format C3 convert to e.g. 48

    note_names = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
    note_name = note_str[:-1]
    octave = int(note_str[-1])
    return note_names.index(note_name) + (octave + 1) * 12

class MidiProcessor:
        def __init__(self,channel=0, note_range=(ToNoteValue("C3"), ToNoteValue("C8"))):
            self.MIDI_CHANNEL = channel
            self.NOTE_RANGE = note_range
            self.midi_file = None

            try:
                mido.set_backend('mido.backends.rtmidi', load=True)
            except ImportError:
                print("Backend not found. Please run 'pip install -e requirements.txt' to install the required dependencies.")
        
        def load_midi_file(self, input_path):
            self.midi_file = MidiFile(input_path)

        def filter_midi_file(self, output_path=None):

            if not self.midi_file:
                print("No MIDI file loaded. Please load a MIDI file before attempting to filter.")
                return None

            """Remove invalid notes and save filtered MIDI file"""
            filtered_file = MidiFile(ticks_per_beat=self.midi_file.ticks_per_beat)
    
            for track in self.midi_file.tracks:
                filtered_track = MidiTrack()
                accumulated_time = 0  # Track time from removed messages
        
                for msg in track:
                    # Keep all non-note messages (tempo, meta events, etc.)
                    if msg.type not in ['note_on', 'note_off']:
                        # Add any accumulated time to this message
                        new_msg = msg.copy(time=msg.time + accumulated_time)
                        filtered_track.append(new_msg)
                        accumulated_time = 0
                        # Keep only valid notes
                    elif Is_valid_note(msg, self.MIDI_CHANNEL, self.NOTE_RANGE):
                        # Add any accumulated time to this message
                        new_msg = msg.copy(time=msg.time + accumulated_time)
                        filtered_track.append(new_msg)
                        accumulated_time = 0
                    else:
                        # Accumulate time from removed messages
                        accumulated_time += msg.time
        
                filtered_file.tracks.append(filtered_track)
    
            if output_path:
                filtered_file.save(output_path)
            return filtered_file

        def play_midi_file(self):
            """Play a MIDI file with proper timing"""

            try:
                start_time = time.time()

                if self.midi_file is None:
                    print("No MIDI file loaded. Please load a MIDI file before attempting to play.")
                    return
                for msg in self.midi_file.play():
                    if not msg.is_meta:
                        print(f"Playing: {msg}")
                    if hasattr(msg, 'time'):
                        time.sleep(msg.time)

                end_time = time.time()
                print(f"Finished playing MIDI file. Total time: {end_time - start_time:.2f} seconds")

            except (AttributeError, OSError) as e:
                print("error while attempting to play MIDI file:", e)

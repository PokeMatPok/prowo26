import midi_api

input_file = "C:\\Users\\Neuron\\Desktop\\prowo26\\rasputin.mid"
output_file = "C:\\Users\\Neuron\\Desktop\\prowo26\\rasputin_filtered.mid"

def main():

    midi_processor = midi_api.MidiProcessor()
    midi_processor.load_midi_file(input_file)
    
    print(f"Filtering MIDI file...")
    print(f"Channel: {midi_processor.MIDI_CHANNEL + 1}, Note range: {midi_processor.NOTE_RANGE[0]}-{midi_processor.NOTE_RANGE[1]}")
    
    # Filter and save
    midi_processor.filter_midi_file(output_file)
    print(f"Filtered file saved to: {output_file}")
    
    # Play the filtered file
    print("\nPlaying filtered file...")
    midi_processor.play_midi_file()

if __name__ == "__main__":
    main()
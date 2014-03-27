
Keyboard Logger
===============

Keyboard and ThinkGear MindSet stream logger allows you to dump
agregated ThinkGear MindSet and Keyboard stream with custom interval.
This dump consists of two streams written to `StreamLogger.txt`
and `WordsStream.txt` files in current directory. Each time `kl.exe`
is started these files are appended.

`StreamLogger.txt` is updated with 10ms interval and holds
the current state of Keyboard and the current state of MindSet.
CSV formar of agregated stream:

	Name,KeyLayout,Timestamp,Keystate,Att,Med,ALow,AHigh,BLow,BHigh,GLow,GHigh,D,T

`WordsStream.txt` is sequence of words delimited by spaces or other trim symbols.
Words CSV header:

	Name,TimestampBegin,Duration,Word

Usage
-----

To run keylogger just run `kl.exe`.
To unload please use Taskbar.
The application supports UTF-8 and Input Layout sensitive even in browser tab windows.
Cannot be runned twice.

Credits
-------

* Denis Poltavets
* Maxim Sokhatsky

OM A HUM

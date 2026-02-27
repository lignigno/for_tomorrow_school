package dfd

/*
#cgo CFLAGS: -I/Users/Guest/SDL/include
#cgo LDFLAGS: -L/Users/Guest/SDL/lib -Wl,-rpath,/Users/Guest/SDL/lib -lSDL3
#include "dfd.h"
*/
import "C"

import (
	"runtime"
	"unsafe"
)

type (
	Surface             []uint32
	WindowID            uint32
	KeyboardID          uint32
	Keycode             uint32
	Keymod              uint16
	Scancode            uint32
	MouseID             uint32
	MouseButtonFlags    uint32
	MouseWheelDirection bool
	EventType           uint32
)

type Window struct {
	_w           int
	_h           int
	_surfaceSize int
	_win         unsafe.Pointer // internal data on C
	_windowId    WindowID

	Width   int
	Height  int
	Surface Surface
	ID      WindowID // The value 0 is an invalid ID
}

type EventContainer struct {
	_cevent C.SDL_Event

	Type             EventType         // Event type, shared with all events, uint32 to cover user events which are not in the SDL_EventType enumeration
	Timestamp        uint64            // In nanoseconds
	ID               WindowID          // The associated window
	WindowEvent      *WindowEvent      // Window event data
	KeyboardEvent    *KeyboardEvent    // Keyboard event data
	MouseMotionEvent *MouseMotionEvent // Mouse motion event data
	MouseButtonEvent *MouseButtonEvent // Mouse button event data
	MouseWheelEvent  *MouseWheelEvent  // Mouse wheel event data
	DropEvent        *DropEvent        // Drag and drop event data
}

type WindowEvent struct {
	Data1 int32 // Event dependent data
	Data2 int32 // Event dependent data
}

type KeyboardEvent struct {
	Which    KeyboardID // The keyboard instance id, or 0 if unknown or virtual
	Scancode Scancode   // SDL physical key code
	Key      Keycode    // SDL virtual key code
	Mod      Keymod     // Current key modifiers
	Raw      uint16     // The platform dependent scancode for this event
	Down     bool       // `true` if the key is pressed
	Repeat   bool       // `true` if this is a key repeat
}

type MouseMotionEvent struct {
	Which MouseID          // I didn't understand, if you want to figure it out, then RTFM SDL_MouseMotionEvent
	State MouseButtonFlags // The current button state
	X     float64          // X coordinate, relative to window
	Y     float64          // Y coordinate, relative to window
	Xrel  float64          // The relative motion in the X direction
	Yrel  float64          // The relative motion in the Y direction
}

type MouseButtonEvent struct {
	Which  MouseID // I didn't understand, if you want to figure it out, then RTFM SDL_MouseButtonEvent
	Button uint8   // The mouse button index
	Down   bool    // `true` if the button is pressed
	Clicks uint8   // 1 for single-click, 2 for double-click, etc.
	X      float64 // X coordinate, relative to window
	Y      float64 // Y coordinate, relative to window
}

type MouseWheelEvent struct {
	Which     MouseID             // I didn't understand, if you want to figure it out, then RTFM SDL_MouseWheelEvent
	X         float64             // The amount scrolled horizontally, positive to the right and negative to the left
	Y         float64             // The amount scrolled vertically, positive away from the user and negative toward the user
	Direction MouseWheelDirection // Set to `true` when FLIPPED. When FLIPPED the values in X and Y will be opposite. Multiply by -1 to change them back
	Mouse_x   float64             // X coordinate, relative to window
	Mouse_y   float64             // Y coordinate, relative to window
	Integer_x int32               // The amount scrolled horizontally, accumulated to whole scroll "ticks"
	Integer_y int32               // The amount scrolled vertically, accumulated to whole scroll "ticks"
}

type DropEvent struct {
	X      float64 // X coordinate, relative to window (not on begin)
	Y      float64 // Y coordinate, relative to window (not on begin)
	Source string  // maybe later
	Data   string  // maybe later
}

const (
	SUCCESS = 0
	FAILURE = 1
)

const (
	EVENT_QUIT             = C.SDL_EVENT_QUIT
	EVENT_KEYMAP_CHANGED   = C.SDL_EVENT_KEYMAP_CHANGED
	EVENT_CLIPBOARD_UPDATE = C.SDL_EVENT_CLIPBOARD_UPDATE

	EVENT_WINDOW_MOVED           = C.SDL_EVENT_WINDOW_MOVED
	EVENT_WINDOW_MINIMIZED       = C.SDL_EVENT_WINDOW_MINIMIZED
	EVENT_WINDOW_RESTORED        = C.SDL_EVENT_WINDOW_RESTORED
	EVENT_WINDOW_MOUSE_ENTER     = C.SDL_EVENT_WINDOW_MOUSE_ENTER
	EVENT_WINDOW_MOUSE_LEAVE     = C.SDL_EVENT_WINDOW_MOUSE_LEAVE
	EVENT_WINDOW_FOCUS_GAINED    = C.SDL_EVENT_WINDOW_FOCUS_GAINED
	EVENT_WINDOW_FOCUS_LOST      = C.SDL_EVENT_WINDOW_FOCUS_LOST
	EVENT_WINDOW_CLOSE_REQUESTED = C.SDL_EVENT_WINDOW_CLOSE_REQUESTED
	EVENT_WINDOW_OCCLUDED        = C.SDL_EVENT_WINDOW_OCCLUDED

	EVENT_KEY_DOWN = C.SDL_EVENT_KEY_DOWN
	EVENT_KEY_UP   = C.SDL_EVENT_KEY_UP

	EVENT_MOUSE_MOTION = C.SDL_EVENT_MOUSE_MOTION

	EVENT_MOUSE_BUTTON_DOWN = C.SDL_EVENT_MOUSE_BUTTON_DOWN
	EVENT_MOUSE_BUTTON_UP   = C.SDL_EVENT_MOUSE_BUTTON_UP

	EVENT_MOUSE_WHEEL = C.SDL_EVENT_MOUSE_WHEEL

	EVENT_DROP_FILE     = C.SDL_EVENT_DROP_FILE
	EVENT_DROP_TEXT     = C.SDL_EVENT_DROP_TEXT
	EVENT_DROP_BEGIN    = C.SDL_EVENT_DROP_BEGIN
	EVENT_DROP_COMPLETE = C.SDL_EVENT_DROP_COMPLETE
	EVENT_DROP_POSITION = C.SDL_EVENT_DROP_POSITION
)

// __________________________________________________________________________________FUNCS

func init() {
	runtime.LockOSThread()

	if retcode := C.DfD_init(); retcode != SUCCESS {
		panic("dfd can't init")
	}
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

func CreateWindow(title string, w, h int) *Window {
	cStr := C.CString(title)
	defer C.free(unsafe.Pointer(cStr))

	newWin := &Window{
		Width:        w,
		Height:       h,
		_w:           w,
		_h:           h,
		_win:         C.DfD_createWindow(cStr, C.int(w), C.int(h)),
		_surfaceSize: w * h,
	}

	if newWin._win == nil {
		return nil
	}

	newWin._windowId = WindowID(C.DfD_get_window_id(newWin._win))
	newWin.ID = newWin._windowId
	newWin.Surface = make(Surface, newWin._surfaceSize)

	return newWin
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

func (this *Window) Clear() {
	for i := 0; i < len(this.Surface); i++ {
		this.Surface[i] = 0x0
	}
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// Render window surface. Returned value:
//
//	-dfd.SUCCESS
//	-dfd.FAILURE
func (this *Window) Update() int {
	if len(this.Surface) < this._surfaceSize {
		return FAILURE
	}

	surfacePtr := (*C.uint32_t)(unsafe.Pointer(&this.Surface[0]))
	if int(C.DfD_update(this._win, surfacePtr)) != SUCCESS {
		return FAILURE
	}

	return SUCCESS
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

func (this *Window) _Hide() {
	// maybe do
	// hide window
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

func (this *Window) _Show() {
	// maybe do
	// show window
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

func (this *Window) RestoreParams() {
	this.Width = this._w
	this.Height = this._h
	this.ID = this._windowId
	this.Surface = make(Surface, this._surfaceSize)
}

//                                                                                       |
// --------------------------------------------------------------------------------------|
//                                                                                       |

// Example:
//
//	for game_is_still_running {
//		var event dfd.Event
//		for dfd.PollEvent(&event) {  // poll until all events are handled!
//			// decide what to do with this event.
//		}
//		// update game state, draw the current frame
//	}
func PollEvent(event *EventContainer) bool {
	if event == nil {
		if int(C.DfD_pool_event(unsafe.Pointer(nil))) != SUCCESS {
			return false
		}

		return true
	}

	event.WindowEvent = nil
	event.KeyboardEvent = nil
	event.MouseMotionEvent = nil
	event.MouseButtonEvent = nil
	event.MouseWheelEvent = nil
	event.DropEvent = nil

	for {
		if int(C.DfD_pool_event(unsafe.Pointer(&event._cevent))) != SUCCESS {
			return false
		}

		common := (*struct {
			Type      EventType
			_reserved uint32
			Timestamp uint64
			_         [112]byte
		})(unsafe.Pointer(&event._cevent))

		event.Type = common.Type
		event.Timestamp = common.Timestamp
		event.ID = 0

		if fillEventContainer(event) {
			return true
		}
	}
}

func fillEventContainer(event *EventContainer) bool {
	switch event.Type {
	case EVENT_QUIT, EVENT_KEYMAP_CHANGED, EVENT_CLIPBOARD_UPDATE:
		return true
	case EVENT_WINDOW_MOVED, EVENT_WINDOW_MINIMIZED, EVENT_WINDOW_RESTORED,
		EVENT_WINDOW_MOUSE_ENTER, EVENT_WINDOW_MOUSE_LEAVE, EVENT_WINDOW_FOCUS_GAINED,
		EVENT_WINDOW_FOCUS_LOST, EVENT_WINDOW_CLOSE_REQUESTED, EVENT_WINDOW_OCCLUDED:
		converter := (*C.SDL_WindowEvent)(unsafe.Pointer(&event._cevent))
		event.WindowEvent = &WindowEvent{
			Data1: int32(converter.data1),
			Data2: int32(converter.data2),
		}
	case EVENT_KEY_DOWN, EVENT_KEY_UP:
		converter := (*C.SDL_KeyboardEvent)(unsafe.Pointer(&event._cevent))
		event.KeyboardEvent = &KeyboardEvent{
			Which:    KeyboardID(converter.which),
			Scancode: Scancode(converter.scancode),
			Key:      Keycode(converter.key),
			Mod:      Keymod(converter.mod),
			Raw:      uint16(converter.raw),
			Down:     bool(converter.down),
			Repeat:   bool(converter.repeat),
		}
	case EVENT_MOUSE_MOTION:
		converter := (*C.SDL_MouseMotionEvent)(unsafe.Pointer(&event._cevent))
		event.MouseMotionEvent = &MouseMotionEvent{
			Which: MouseID(converter.which),
			State: MouseButtonFlags(converter.state),
			X:     float64(converter.x),
			Y:     float64(converter.y),
			Xrel:  float64(converter.xrel),
			Yrel:  float64(converter.yrel),
		}
	case EVENT_MOUSE_BUTTON_DOWN, EVENT_MOUSE_BUTTON_UP:
		converter := (*C.SDL_MouseButtonEvent)(unsafe.Pointer(&event._cevent))
		event.MouseButtonEvent = &MouseButtonEvent{
			Which:  MouseID(converter.which),
			Button: uint8(converter.button),
			Down:   bool(converter.down),
			Clicks: uint8(converter.clicks),
			X:      float64(converter.x),
			Y:      float64(converter.y),
		}
	case EVENT_MOUSE_WHEEL:
		converter := (*C.SDL_MouseWheelEvent)(unsafe.Pointer(&event._cevent))
		event.MouseWheelEvent = &MouseWheelEvent{
			Which:     MouseID(converter.which),
			X:         float64(converter.x),
			Y:         float64(converter.y),
			Direction: MouseWheelDirection(converter.direction == C.SDL_MOUSEWHEEL_FLIPPED),
			Mouse_x:   float64(converter.mouse_x),
			Mouse_y:   float64(converter.y),
			Integer_x: int32(converter.integer_x),
			Integer_y: int32(converter.y),
		}
	case EVENT_DROP_FILE, EVENT_DROP_TEXT, EVENT_DROP_BEGIN,
		EVENT_DROP_COMPLETE, EVENT_DROP_POSITION:
		converter := (*C.SDL_DropEvent)(unsafe.Pointer(&event._cevent))
		event.DropEvent = &DropEvent{
			X:      float64(converter.x),
			Y:      float64(converter.y),
			Source: "maybe later",
			Data:   "maybe later",
		}
	default:
		return false
	}

	return true
}

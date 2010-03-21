#pragma once;

class BitmapRenderer : public htmlayout::event_handler
{
public:

	typedef boost::signals2::signal
		< void(size_t, HBITMAP &)
		> BitmapSignal;
	typedef boost::signals2::signal
		< void(size_t, HBITMAP &)
		>::slot_type BitmapSlot;

private:

	HINSTANCE instance;

	BitmapSignal SignalLoadBitmap;

public:

	BitmapRenderer(HINSTANCE instance);

	void ConnectLoadBitmap(BitmapSlot OnLoadBitmap);

	virtual BOOL handle_draw(HELEMENT he, DRAW_PARAMS & params);
};


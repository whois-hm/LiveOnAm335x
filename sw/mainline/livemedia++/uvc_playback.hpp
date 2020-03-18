#pragma once

class uvc_playback :
		public playback_inst
{

private:

	uvc *_uvc;
	std::mutex _lock;

public:
	uvc_playback(const avattr &attr, char const *name) :
		playback_inst(attr),
		_uvc(nullptr)
	{
		DECLARE_THROW(attr.notfound(avattr_key::frame_video), "uvc playback can't found video frame");
		_uvc = new uvc(name);
		pause();
	}
	virtual ~uvc_playback()
	{
		delete _uvc;
	}
	enum AVMediaType get_master_clock()
	{
		return AVMEDIA_TYPE_VIDEO;
	}
	void pause()
	{
		if(_lock.try_lock())
		{
			_uvc->stop();
		}
	}
	void resume(bool closing = false)
	{
		_lock.unlock();
	}
	void seek(double incr)
	{
		/*can't seek camera*/
	}

	bool has(avattr::avattr_type_string &&key)
	{
		if(key == avattr_key::frame_video)
		{
			return true;
		}
		return false;
	}
	void play()
	{
		resume();
	}
    duration_div duration()
    {
    	/*can't know duration */
    	return duration_div (-1,-1,-1,-1,-1);
    }
	virtual int take(const std::string &title, pixel &output)
	{

		autolock a(_lock);
		/*
		 	 we do not have to pts scheduling.
		 	 just schedule dependent uvc driver's fps
		 */
		int res = _uvc->waitframe(5000);
		if(res > 0)
		{

			if(_uvc->get_videoframe([&](pixelframe &pix)->void{
					swxcontext_class (pix, (_attr));
					pix >> output;
				}) <= 0)
			{
				res = -1;
			}

			if(!output.can_take())
			{

				/*
				 	 internal error
				 */
				res = -1;
			}

		}
		return res;
	}
	virtual int take(const std::string &title, pcm_require &output)
	{
		return -1;
	}
};




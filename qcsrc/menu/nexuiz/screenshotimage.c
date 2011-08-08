#ifdef INTERFACE
CLASS(NexuizScreenshotImage) EXTENDS(Image)
	METHOD(NexuizScreenshotImage, configureNexuizScreenshotImage, void(entity, string))
	METHOD(NexuizScreenshotImage, draw, void(entity))
	ATTRIB(NexuizScreenshotImage, focusable, float, 1) // mousePress and mouseDrag work only if focusable is set
	METHOD(NexuizScreenshotImage, mousePress, float(entity, vector))
	METHOD(NexuizScreenshotImage, mouseDrag, float(entity, vector))
	METHOD(NexuizScreenshotImage, mouseMove, float(entity, vector))
	METHOD(NexuizScreenshotImage, resizeNotify, void(entity, vector, vector, vector, vector))
	ATTRIB(NexuizScreenshotImage, realFontSize, vector, '0 0 0')
	ATTRIB(NexuizScreenshotImage, fontSize, float, SKINFONTSIZE_NORMAL)
	ATTRIB(NexuizScreenshotImage, showTitle, float, 1)
	ATTRIB(NexuizScreenshotImage, screenshotTime, float, 0)
	ATTRIB(NexuizScreenshotImage, screenshotTitle, string, string_null)
ENDCLASS(NexuizScreenshotImage)
entity makeNexuizScreenshotImage();
#endif

#ifdef IMPLEMENTATION
entity makeNexuizScreenshotImage()
{
	entity me;
	me = spawnNexuizScreenshotImage();
	me.configureNexuizScreenshotImage(me, string_null);
	return me;
}

void configureNexuizScreenshotImageNexuizScreenshotImage(entity me, string theImage)
{
	me.configureImage(me, theImage);
	me.forcedAspect = -1;
	me.screenshotTime = time;
	me.updateAspect(me);
	if (me.screenshotTitle)
		strunzone(me.screenshotTitle);
	me.screenshotTitle = strzone(substring(me.src, 13, strlen(theImage) - 13)); //strip "/screenshots/"
}

float mousePressNexuizScreenshotImage(entity me, vector coords)
{
	return me.drag_setStartPos(me, coords);
}

float mouseDragNexuizScreenshotImage(entity me, vector coords)
{
	return me.drag(me, coords);
}

float mouseMoveNexuizScreenshotImage(entity me, vector coords)
{
	return me.drag_setStartPos(me, coords);
}

void drawNexuizScreenshotImage(entity me)
{
	if (me.src != "")
	{
		float theAlpha;
		drawImage(me);
		if (me.showTitle && time < me.screenshotTime + 4) // 3 seconds at full alpha, 1 second fading out
		{
			theAlpha = (4 - (time - me.screenshotTime));
			draw_CenterText('0.5 0 0', me.screenshotTitle, me.realFontSize, '1 1 1', theAlpha, FALSE);
		}
		if (time < me.zoomTime + 2) // 1 seconds at full alpha, 1 second fading out
		{
			theAlpha = (2 - (time - me.zoomTime));
			draw_Text('0.05 0.95 0', strcat(ftos(me.zoomFactor * 100), "%"), me.realFontSize, '1 1 1', theAlpha, FALSE);
		}
	}
}

void resizeNotifyNexuizScreenshotImage(entity me, vector relOrigin, vector relSize, vector absOrigin, vector absSize)
{
	resizeNotifyImage(me, relOrigin, relSize, absOrigin, absSize);
	me.realFontSize_y = me.fontSize / absSize_y;
	me.realFontSize_x = me.fontSize / absSize_x;
}
#endif

//
//  Event.h
//  CameraDetection
//
//  Created by Peter Huang on 25/04/2015.
//  Copyright (c) 2015 Peter Huang. All rights reserved.
//

#pragma once

class Event
{
public:
	enum Type
	{
		HSVLowGroupChanged,
		HSVHighGroupChanged,
		MousePressedOnView,
		MouseReleasedOnview
	};
	Event(Type type):m_type(type){};
	virtual ~Event(){};
	Type type() const { return m_type; };
	
protected:
	Type m_type;
};

class HSVControlEvent: public Event
{
public:
	HSVControlEvent(Type type, int parameterID, int value)
	:Event(type),m_id(parameterID),m_value(value){};
	~HSVControlEvent(){};
	int getID() const { return m_id; };
	int value() const { return m_value; };
	
private:
	int m_id;
	int m_value;
};

class MouseEvent: public Event
{
public:
	MouseEvent(Type type, int x, int y)
	:Event(type),m_x(x),m_y(y){};
	~MouseEvent(){};
	int getX() const { return m_x; };
	int getY() const { return m_y; };
	
private:
	int m_x;
	int m_y;
};

/*
 Copyright (C) 2015 Stephen Braitsch [http://braitsch.io]
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#pragma once
#include "ofxDatGuiComponent.h"
#include "ofxDatGuiTextInputField.h"

class ofxDatGuiSlider : public ofxDatGuiComponent {
    
public:
    
    ofxDatGuiSlider(string label, float min, float max, double val) : ofxDatGuiComponent(label)
    {
        mMin = min;
        mMax = max;
        setPrecision(2);
        mType = ofxDatGuiType::SLIDER;
        mInput = new ofxDatGuiTextInputField();
        mInput->setTextInputFieldType(ofxDatGuiInputType::NUMERIC);
        mInput->onInternalEvent(this, &ofxDatGuiSlider::onInputChanged);
        setTheme(ofxDatGuiComponent::getTheme());
        setValue(val);
        setWireConnectionType();
    }
    
    ofxDatGuiSlider(string label, float min, float max, string type) : ofxDatGuiComponent(label)
    {
        mMin = min;
        mMax = max;
        setPrecision(2);
        mType = ofxDatGuiType::SLIDER;
        mInput = new ofxDatGuiTextInputField();
        mInput->setTextInputFieldType(ofxDatGuiInputType::NUMERIC);
        mInput->onInternalEvent(this, &ofxDatGuiSlider::onInputChanged);
        setTheme(ofxDatGuiComponent::getTheme());
        setWireConnectionType();
    }
    
    ofxDatGuiSlider(string label, float min, float max) : ofxDatGuiSlider(label, min, max, (max+min)/2) {
    }
    ofxDatGuiSlider(ofParameter<int> & p) : ofxDatGuiSlider(p.getName(), p.getMin(), p.getMax(), p.get()) {
        mParamI = &p;
        setPrecision(0);
        calculateScale();
        mParamI->addListener(this, &ofxDatGuiSlider::onParamI);
        setWireConnectionType();
    }
    ofxDatGuiSlider(ofParameter<float> & p) : ofxDatGuiSlider(p.getName(), p.getMin(), p.getMax(), p.get()) {
        mParamF = &p;
        setPrecision(2);
        calculateScale();
        mParamF->addListener(this, &ofxDatGuiSlider::onParamF);
        setWireConnectionType();
    }
    
    ~ofxDatGuiSlider()
    {
        delete mInput;
    }
    
    void setWireConnectionType()
    {
        inputConnection->setWireConnectionType(ConnectionType::DK_SCALE);
        outputConnection->setWireConnectionType(ConnectionType::DK_SCALE);
    }
    void setTheme(const ofxDatGuiTheme* theme)
    {
        setComponentStyle(theme);
        mSliderFill = theme->color.slider.fill;
        mBackgroundFill = theme->color.inputAreaBackground;
        mStyle.stripe.color = theme->stripe.slider;
        mInput->setTheme(theme);
        mInput->setTextInactiveColor(theme->color.slider.text);
        setWidth(theme->layout.width, theme->layout.labelWidth);
    }
    
    void setWidth(int width, float labelWidth)
    {
        ofxDatGuiComponent::setWidth(width, labelWidth);
        float totalWidth = mStyle.width - mLabel.width;
        mSliderWidth = totalWidth * 0.70;
        mInputX = mLabel.width + mSliderWidth + mStyle.padding;
        mInputWidth = totalWidth - mSliderWidth - (mStyle.padding * 2);
        mInput->setWidth(mInputWidth);
        mInput->setPosition(x + mInputX, y + mStyle.padding);
    }
    
    void setPosition(int x, int y)
    {
        ofxDatGuiComponent::setPosition(x, y);
        mInput->setPosition(x + mInputX, y + mStyle.padding);
        
        ofPoint p1;
        p1.x = (int) x - 20;
        p1.y = (int) y+mStyle.padding + 15;
        inputConnection->setup(p1, this->getName());
        
        ofPoint p2;
        p2.x = (int) x+mLabel.width + mSliderWidth + mInput->getWidth() + 25;
        p2.y = (int) y+mStyle.padding + 15;
        outputConnection->setup(p2, this->getName());
        
        inputConnection->setScale(&mScale);
        outputConnection->setScale(&mScale);
    }
    
    ofxDatGuiSlider * setPrecision(int precision, bool truncateValue = true)
    {
        mPrecision = precision;
        mTruncateValue = truncateValue;
        if (mPrecision > MAX_PRECISION) mPrecision = MAX_PRECISION;
        return this;
    }
    
    int getPrecision()
    {
        return mPrecision;
    }
    
    void setMin(float min)
    {
        mMin = min;
        if (mMin < mMax){
            calculateScale();
        }   else{
            onInvalidMinMaxValues();
        }
    }
    
    void setMax(float max)
    {
        mMax = max;
        if (mMax > mMin){
            calculateScale();
        }   else{
            onInvalidMinMaxValues();
        }
    }
    
    void setValue(double value)
    {
        mValue = value;
        if (mValue > mMax){
            mValue = mMax;
        }   else if (mValue < mMin){
            mValue = mMin;
        }
        if (mTruncateValue) mValue = round(mValue, mPrecision);
        calculateScale();
    }
    
    double getValue()
    {
        return mValue;
    }
    
    float * getBoundf()
    {
        return mBoundf;
    }
    
    int * getBoundi()
    {
        return mBoundi;
    }
    
    float getComponentScale()
    {
        return mScale;
    }
    
    void printValue()
    {
        if (mTruncateValue == false){
            cout << setprecision(16) << getValue() << endl;
        }   else{
            int n = ofToString(mValue).find(".");
            if (n == -1) n = ofToString(mValue).length();
            cout << setprecision(mPrecision + n) << mValue << endl;
        }
    }
    
    void setComponentScale(double scale)
    {
        mScale = scale;
        if(mBoundi != nullptr) {
            int sc = ((mMax-mMin) * mScale) + mMin;
            *mBoundi = sc;
        } else if (mBoundf != nullptr)
        {
            float sc = ((mMax-mMin) * mScale) + mMin;
            *mBoundf = sc;
        }
        
        
    }
    
    void setScale(double scale)
    {
        mScale = scale;
        if (mScale < 0 || mScale > 1){
            ofLogError() << "row #" << mIndex << " : scale must be between 0 & 1" << " [setting to 50%]";
            mScale = 0.5f;
        }
        mValue = ((mMax-mMin) * mScale) + mMin;
    }
    
    double getScale()
    {
        return mScale;
    }
    
    /*
     variable binding methods
     */
    void bind(int &val)
    {
        mBoundi = &val;
        mBoundf = nullptr;
    }
    
    void bind(float &val)
    {
        mBoundf = &val;
        mBoundi = nullptr;
    }
    
    void bind(int &val, int min, int max)
    {
        mMin = min;
        mMax = max;
        mBoundi = &val;
        mBoundf = nullptr;
    }
    
    void bind(float &val, float min, float max)
    {
        mMin = min;
        mMax = max;
        mBoundf = &val;
        mBoundi = nullptr;
    }
    
    void update(bool acceptEvents = true)
    {
        
        ofxDatGuiComponent::update(acceptEvents);
        // check for variable bindings //
        if (mBoundf != nullptr && !mInput->hasFocus()) {
            setValue(*mBoundf);
        }   else if (mBoundi != nullptr && !mInput->hasFocus()){
            setValue(*mBoundi);
        }
    }
    
    void draw()
    {
        if (!mVisible) return;
        ofPushStyle();
        ofxDatGuiComponent::draw();
        // slider bkgd //
        ofSetColor(mBackgroundFill);
        ofDrawRectangle(x+mLabel.width, y+mStyle.padding, mSliderWidth, mStyle.height-(mStyle.padding*2));
        // slider fill //
        if (mScale > 0){
            ofSetColor(mSliderFill);
            ofDrawRectangle(x+mLabel.width, y+mStyle.padding, mSliderWidth*mScale, mStyle.height-(mStyle.padding*2));
        }
        
        mInput->draw();
        // numeric input field //
        
        if(getMidiMode())
        {
            ofSetColor(getMidiMap() ? ofColor(70, 128) : ofColor(0, 200));
            ofDrawRectangle(x+mLabel.width, y+mStyle.padding, mSliderWidth, mStyle.height-(mStyle.padding*2));
            
            ofDrawRectangle(x+mLabel.width + mSliderWidth+(mStyle.padding), y+mStyle.padding, mInput->getWidth(), mStyle.height-(mStyle.padding*2));
            ofSetColor(ofColor(255));
            
            mFont->draw(mappingString, x+mLabel.width + 5, y+mStyle.padding + mStyle.height/2 + 2);
            
            outputConnection->draw();
            inputConnection->draw();
            
        }
        
        ofPopStyle();
    }
    
    bool hitTest(ofPoint m)
    {
        if (!mEnabled || !mVisible){
            return false;
        }   else if (m.x>=x+mLabel.width && m.x<= x+mLabel.width+mSliderWidth && m.y>=y+mStyle.padding && m.y<= y+mStyle.height-mStyle.padding){
            return true;
        }   else if (mInput->hitTest(m)){
            return true;
        }   else{
            return false;
        }
    }
    
    static ofxDatGuiSlider* getInstance() { return new ofxDatGuiSlider("X", 0, 100); }
protected:
    
    
    
    void onMousePress(ofPoint m)
    {
        ofxDatGuiComponent::onMousePress(m);
        
        if (mInput->hitTest(m)){
            mInput->onFocus();
        }   else if (mInput->hasFocus()){
            mInput->onFocusLost();
        }
    }
    
    void onMouseDrag(ofPoint m)
    {
        if (mFocused && mInput->hasFocus() == false){
            float s = (m.x-x-mLabel.width)/mSliderWidth;
            if (s > .999) s = 1;
            if (s < .001) s = 0;
            // don't dispatch an event if scale hasn't changed //
            if (s == mScale) return;
            mScale = s;
            mValue = ((mMax-mMin) * mScale) + mMin;
            if (mTruncateValue) mValue = round(mValue, mPrecision);
            setTextInput();
            dispatchSliderChangedEvent();
        }
    }
    
    void onMouseRelease(ofPoint m)
    {
        ofxDatGuiComponent::onMouseRelease(m);
        if (mInput->hitTest(m) == false) onFocusLost();
    }
    
    void onFocusLost()
    {
        ofxDatGuiComponent::onFocusLost();
        if (mInput->hasFocus()) mInput->onFocusLost();
    }
    
    void onKeyPressed(int key)
    {
        if (mInput->hasFocus()) mInput->onKeyPressed(key);
    }
    
    void onInputChanged(ofxDatGuiInternalEvent e)
    {
        setValue(ofToFloat(mInput->getText()));
        dispatchSliderChangedEvent();
    }
    
    void dispatchSliderChangedEvent()
    {
        // update any bound variables //
        if (mBoundf != nullptr) {
            *mBoundf = mValue;
        }   else if (mBoundi != nullptr) {
            *mBoundi = mValue;
        }   else if (mParamI != nullptr) {
            mParamI->set(mValue);
        }   else if (mParamF != nullptr) {
            mParamF->set(mValue);
        }
        // dispatch event out to main application //
        if (sliderEventCallback != nullptr) {
            ofxDatGuiSliderEvent e(this, mValue, mScale);
            sliderEventCallback(e);
        }   else{
            ofxDatGuiLog::write(ofxDatGuiMsg::EVENT_HANDLER_NULL);
        }
    }
    
private:
    
    string  inputType;
    
    float   mMin;
    float   mMax;
    double  mValue;
    double  mScale;
    int     mPrecision;
    bool    mTruncateValue;
    int     mInputX;
    int     mInputWidth;
    int     mSliderWidth;
    ofColor mSliderFill;
    ofColor mBackgroundFill;
    ofxDatGuiTextInputField* mInput;
        
    static const int MAX_PRECISION = 4;
    
    int*    mBoundi = nullptr;
    float*  mBoundf = nullptr;
    ofParameter<int>* mParamI = nullptr;
    ofParameter<float>* mParamF = nullptr;
    void onParamI(int& n) { setValue(n); }
    void onParamF(float& n) { setValue(n); }
    
    void calculateScale()
    {
        mScale = ofxDatGuiScale(mValue, mMin, mMax);
        setTextInput();
    }
    
    
    void setTextInput()
    {
        string v = ofToString(round(mValue, mPrecision));
        if (mValue != mMin && mValue != mMax){
            int p = v.find('.');
            if (p == -1 && mPrecision != 0){
                v+='.';
                p = v.find('.');
            }
            while(v.length() - p < (mPrecision + 1)) v+='0';
        }
        mInput->setText(v);
    }
    
    double round(double num, int precision)
    {
        return roundf(num * pow(10, precision)) / pow(10, precision);
    }
    
    void onInvalidMinMaxValues()
    {
        ofLogError() << "row #" << mIndex << " : invalid min & max values" << " [setting to 50%]";
        mMin = 0;
        mMax = 100;
        mScale = 0.5f;
        mValue = (mMax+mMin) * mScale;
    }
    
    string getInputType()
    {
        return inputType;
    }
};


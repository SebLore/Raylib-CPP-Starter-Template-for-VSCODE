/**
 * @file GUIComponents.h
 * @brief GUI component implementations
 * @date 2025-07-12
 * @details This file contains implementations of various GUI components like buttons, labels, checkboxes, etc.
 */

#pragma once

#include "GUI.h"
#include <string>
#include <iostream>
#include <algorithm>
#include <raylib.h>

// GUI Layout Constants
namespace GUIConstants
{
    // Font sizes
    static constexpr int DEFAULT_FONT_SIZE = 20;
    static constexpr int MAX_FONT_SIZE = 20;
    static constexpr int MIN_FONT_SIZE = 8;
    static constexpr int LABEL_FONT_SIZE = 18;
    static constexpr int SLIDER_LABEL_FONT_SIZE = 16;
    static constexpr int SLIDER_VALUE_FONT_SIZE = 14;
    static constexpr int NAV_BUTTON_FONT_SIZE = 16;

    // Padding and spacing
    static constexpr int TEXT_PADDING = 10;
    static constexpr int BORDER_THICKNESS = 2;
    static constexpr int CHECKBOX_SIZE = 20;
    static constexpr int CHECKBOX_SPACING = 30;
    static constexpr int CHECKBOX_CHECK_PADDING = 4;
    static constexpr int CHECKBOX_CHECK_SIZE = 12;

    // Slider constants
    static constexpr int SLIDER_TRACK_HEIGHT = 4;
    static constexpr int SLIDER_TRACK_HALF_HEIGHT = 2;
    static constexpr int SLIDER_HANDLE_RADIUS = 8;
    static constexpr int SLIDER_HANDLE_AREA_HEIGHT = 16;
    static constexpr int SLIDER_LABEL_OFFSET = 20;

    // Image browser constants
    static constexpr int NAV_BUTTON_WIDTH = 60;
    static constexpr int NAV_BUTTON_HEIGHT = 20;
    static constexpr int NAV_BUTTON_MARGIN = 10;
    static constexpr int NAV_BUTTON_BOTTOM_OFFSET = 30;
    static constexpr int NAV_BUTTON_RIGHT_OFFSET = 70;
    static constexpr int NAV_BUTTON_TEXT_OFFSET = 15;
    static constexpr int NAV_BUTTON_TEXT_Y_OFFSET = 2;
    static constexpr int IMAGE_PLACEHOLDER_Y_OFFSET = 10;
}

// Helper function to get appropriate font size for text to fit within width
inline int GetFittingFontSize(const std::string &text, int maxWidth, int maxFontSize = GUIConstants::MAX_FONT_SIZE, int minFontSize = GUIConstants::MIN_FONT_SIZE)
{
    int fontSize = maxFontSize;
    int textWidth = MeasureText(text.c_str(), fontSize);

    while (textWidth > maxWidth && fontSize > minFontSize)
    {
        fontSize--;
        textWidth = MeasureText(text.c_str(), fontSize);
    }

    return fontSize;
}

class GUIButton : public IGUIComponent
{
public:
    GUIButton(Rectangle bounds, const std::string &text, Color color = LIGHTGRAY)
        : m_text(text), m_color(color), m_textColor(BLACK), m_hoverColor(GRAY), m_clickedColor(DARKGRAY)
    {
        m_bounds = bounds;
    }

    void Render(Vector2 offset) override
    {
        if (!m_visible)
            return;

        Color currentColor = m_color;
        if (m_isClicked)
            currentColor = m_clickedColor;
        else if (m_isHovered)
            currentColor = m_hoverColor;

        DrawRectangleRec(m_bounds, currentColor);
        DrawRectangleLinesEx(m_bounds, GUIConstants::BORDER_THICKNESS, BLACK);

        // Center the text with automatic size fitting
        int fittingFontSize = GetFittingFontSize(m_text, m_bounds.width - GUIConstants::TEXT_PADDING, GUIConstants::DEFAULT_FONT_SIZE); // Leave padding
        int textWidth = MeasureText(m_text.c_str(), fittingFontSize);
        DrawText(m_text.c_str(),
                 m_bounds.x + (m_bounds.width - textWidth) / 2,
                 m_bounds.y + (m_bounds.height - fittingFontSize) / 2,
                 fittingFontSize, m_textColor);
    }

    void HandleInput(Vector2 offset) override
    {
        if (!m_enabled)
            return;

        Vector2 mousePos = GetMousePosition();
        m_isHovered = CheckCollisionPointRec(mousePos, m_bounds);
        m_isClicked = m_isHovered && IsMouseButtonDown(MOUSE_BUTTON_LEFT);

        if (m_isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (m_onClick)
                m_onClick();
        }
    }

    void SetOnClick(std::function<void()> onClick) { m_onClick = onClick; }

private:
    std::string m_text;
    Color m_color, m_textColor, m_hoverColor, m_clickedColor;
    bool m_isHovered = false;
    bool m_isClicked = false;
    std::function<void()> m_onClick;
};

class GUILabel : public IGUIComponent
{
public:
    GUILabel(Rectangle bounds, const std::string &text, Color textColor = BLACK, int fontSize = GUIConstants::DEFAULT_FONT_SIZE)
        : m_text(text), m_textColor(textColor), m_fontSize(fontSize)
    {
        m_bounds = bounds;
    }

    void Render(Vector2 offset) override
    {
        if (!m_visible)
            return;

        // Calculate appropriate font size to fit within bounds
        int fittingFontSize = GetFittingFontSize(m_text, m_bounds.width, m_fontSize);
        DrawText(m_text.c_str(), m_bounds.x, m_bounds.y, fittingFontSize, m_textColor);
    }

    void HandleInput(Vector2 offset) override
    {
        // Labels don't handle input
    }

    void SetText(const std::string &text) { m_text = text; }
    std::string GetText() const { return m_text; }

private:
    std::string m_text;
    Color m_textColor;
    int m_fontSize;
};

class GUICheckbox : public IGUIComponent
{
public:
    GUICheckbox(Rectangle bounds, const std::string &label, bool checked = false)
        : m_label(label), m_checked(checked), m_checkColor(GREEN)
    {
        m_bounds = bounds;
    }

    void Render(Vector2 offset) override
    {
        if (!m_visible)
            return;

        // Draw checkbox box
        Rectangle checkboxRect = {m_bounds.x, m_bounds.y, GUIConstants::CHECKBOX_SIZE, GUIConstants::CHECKBOX_SIZE};
        DrawRectangleRec(checkboxRect, WHITE);
        DrawRectangleLinesEx(checkboxRect, GUIConstants::BORDER_THICKNESS, BLACK);

        // Draw check mark if checked
        if (m_checked)
        {
            DrawRectangle(m_bounds.x + GUIConstants::CHECKBOX_CHECK_PADDING,
                          m_bounds.y + GUIConstants::CHECKBOX_CHECK_PADDING,
                          GUIConstants::CHECKBOX_CHECK_SIZE,
                          GUIConstants::CHECKBOX_CHECK_SIZE, m_checkColor);
        }

        // Draw label with automatic size fitting
        int labelWidth = m_bounds.width - (GUIConstants::CHECKBOX_SIZE + 15); // Account for checkbox and spacing
        int fittingFontSize = GetFittingFontSize(m_label, labelWidth, GUIConstants::LABEL_FONT_SIZE);
        DrawText(m_label.c_str(), m_bounds.x + GUIConstants::CHECKBOX_SPACING, m_bounds.y + GUIConstants::BORDER_THICKNESS, fittingFontSize, BLACK);
    }

    void HandleInput(Vector2 offset) override
    {
        if (!m_enabled)
            return;

        Vector2 mousePos = GetMousePosition();
        Rectangle checkboxRect = {m_bounds.x, m_bounds.y, GUIConstants::CHECKBOX_SIZE, GUIConstants::CHECKBOX_SIZE};

        if (CheckCollisionPointRec(mousePos, checkboxRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            m_checked = !m_checked;
            if (m_onChanged)
                m_onChanged(m_checked);
        }
    }

    void SetOnChanged(std::function<void(bool)> onChanged) { m_onChanged = onChanged; }
    bool IsChecked() const { return m_checked; }
    void SetChecked(bool checked) { m_checked = checked; }

private:
    std::string m_label;
    bool m_checked;
    Color m_checkColor;
    std::function<void(bool)> m_onChanged;
};

class GUIImageBrowser : public IGUIComponent
{
public:
    GUIImageBrowser(Rectangle bounds, const std::vector<std::string> &imagePaths = {})
        : m_imagePaths(imagePaths), m_currentIndex(0)
    {
        m_bounds = bounds;
        LoadCurrentImage();
    }

    ~GUIImageBrowser()
    {
        if (m_currentTexture.id != 0)
        {
            UnloadTexture(m_currentTexture);
        }
    }

    void Render(Vector2 offset) override
    {
        if (!m_visible)
            return;

        // Draw background
        DrawRectangleRec(m_bounds, LIGHTGRAY);
        DrawRectangleLinesEx(m_bounds, GUIConstants::BORDER_THICKNESS, BLACK);

        // Draw current image if available
        if (m_currentTexture.id != 0)
        {
            float scale = std::min(m_bounds.width / m_currentTexture.width,
                                   m_bounds.height / m_currentTexture.height);
            float scaledWidth = m_currentTexture.width * scale;
            float scaledHeight = m_currentTexture.height * scale;

            Rectangle destRect = {
                m_bounds.x + (m_bounds.width - scaledWidth) / 2,
                m_bounds.y + (m_bounds.height - scaledHeight) / 2,
                scaledWidth,
                scaledHeight};

            DrawTexturePro(m_currentTexture,
                           {0, 0, (float)m_currentTexture.width, (float)m_currentTexture.height},
                           destRect, {0, 0}, 0, WHITE);
        }
        else
        {
            // Draw placeholder text
            const char *placeholder = "No Image";
            int textWidth = MeasureText(placeholder, GUIConstants::DEFAULT_FONT_SIZE);
            DrawText(placeholder,
                     m_bounds.x + (m_bounds.width - textWidth) / 2,
                     m_bounds.y + m_bounds.height / 2 - GUIConstants::IMAGE_PLACEHOLDER_Y_OFFSET,
                     GUIConstants::DEFAULT_FONT_SIZE, DARKGRAY);
        }

        // Draw navigation buttons if multiple images
        if (m_imagePaths.size() > 1)
        {
            Rectangle prevButton = {m_bounds.x + GUIConstants::NAV_BUTTON_MARGIN,
                                    m_bounds.y + m_bounds.height - GUIConstants::NAV_BUTTON_BOTTOM_OFFSET,
                                    GUIConstants::NAV_BUTTON_WIDTH,
                                    GUIConstants::NAV_BUTTON_HEIGHT};
            Rectangle nextButton = {m_bounds.x + m_bounds.width - GUIConstants::NAV_BUTTON_RIGHT_OFFSET,
                                    m_bounds.y + m_bounds.height - GUIConstants::NAV_BUTTON_BOTTOM_OFFSET,
                                    GUIConstants::NAV_BUTTON_WIDTH,
                                    GUIConstants::NAV_BUTTON_HEIGHT};

            DrawRectangleRec(prevButton, BLUE);
            DrawRectangleRec(nextButton, BLUE);
            DrawText("Prev", prevButton.x + GUIConstants::NAV_BUTTON_TEXT_OFFSET, prevButton.y + GUIConstants::NAV_BUTTON_TEXT_Y_OFFSET, GUIConstants::NAV_BUTTON_FONT_SIZE, WHITE);
            DrawText("Next", nextButton.x + GUIConstants::NAV_BUTTON_TEXT_OFFSET, nextButton.y + GUIConstants::NAV_BUTTON_TEXT_Y_OFFSET, GUIConstants::NAV_BUTTON_FONT_SIZE, WHITE);
        }
    }

    void HandleInput(Vector2 offset) override
    {
        if (!m_enabled || m_imagePaths.empty())
            return;

        Vector2 mousePos = GetMousePosition();

        if (m_imagePaths.size() > 1)
        {
            Rectangle prevButton = {m_bounds.x + GUIConstants::NAV_BUTTON_MARGIN,
                                    m_bounds.y + m_bounds.height - GUIConstants::NAV_BUTTON_BOTTOM_OFFSET,
                                    GUIConstants::NAV_BUTTON_WIDTH,
                                    GUIConstants::NAV_BUTTON_HEIGHT};
            Rectangle nextButton = {m_bounds.x + m_bounds.width - GUIConstants::NAV_BUTTON_RIGHT_OFFSET,
                                    m_bounds.y + m_bounds.height - GUIConstants::NAV_BUTTON_BOTTOM_OFFSET,
                                    GUIConstants::NAV_BUTTON_WIDTH,
                                    GUIConstants::NAV_BUTTON_HEIGHT};

            if (CheckCollisionPointRec(mousePos, prevButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PreviousImage();
            }
            else if (CheckCollisionPointRec(mousePos, nextButton) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                NextImage();
            }
        }
    }

    void AddImage(const std::string &imagePath)
    {
        m_imagePaths.push_back(imagePath);
        if (m_imagePaths.size() == 1)
        {
            LoadCurrentImage();
        }
    }

    void SetImages(const std::vector<std::string> &imagePaths)
    {
        m_imagePaths = imagePaths;
        m_currentIndex = 0;
        LoadCurrentImage();
    }

private:
    void LoadCurrentImage()
    {
        if (m_currentTexture.id != 0)
        {
            UnloadTexture(m_currentTexture);
            m_currentTexture = {0};
        }

        if (!m_imagePaths.empty() && m_currentIndex < m_imagePaths.size())
        {
            m_currentTexture = LoadTexture(m_imagePaths[m_currentIndex].c_str());
            if (m_currentTexture.id == 0)
            {
                std::cout << "Failed to load image: " << m_imagePaths[m_currentIndex] << std::endl;
            }
        }
    }

    void NextImage()
    {
        if (m_imagePaths.size() > 1)
        {
            m_currentIndex = (m_currentIndex + 1) % m_imagePaths.size();
            LoadCurrentImage();
        }
    }

    void PreviousImage()
    {
        if (m_imagePaths.size() > 1)
        {
            m_currentIndex = (m_currentIndex - 1 + m_imagePaths.size()) % m_imagePaths.size();
            LoadCurrentImage();
        }
    }

    std::vector<std::string> m_imagePaths;
    size_t m_currentIndex;
    Texture2D m_currentTexture = {0};
};

class GUISlider : public IGUIComponent
{
public:
    GUISlider(Rectangle bounds, float minValue, float maxValue, float currentValue, const std::string &label = "")
        : m_minValue(minValue), m_maxValue(maxValue), m_currentValue(currentValue), m_label(label), m_isDragging(false)
    {
        m_bounds = bounds;
        m_currentValue = std::clamp(currentValue, minValue, maxValue);
    }

    void Render(Vector2 offset) override
    {
        if (!m_visible)
            return;

        // Draw slider track
        float trackY = m_bounds.y + m_bounds.height / 2;
        DrawRectangle(m_bounds.x, trackY - GUIConstants::SLIDER_TRACK_HALF_HEIGHT, m_bounds.width, GUIConstants::SLIDER_TRACK_HEIGHT, DARKGRAY);

        // Calculate slider handle position
        float normalizedValue = (m_currentValue - m_minValue) / (m_maxValue - m_minValue);
        float handleX = m_bounds.x + (normalizedValue * m_bounds.width);

        // Draw slider handle
        Color handleColor = m_isDragging ? BLUE : GRAY;
        DrawCircle(handleX, trackY, GUIConstants::SLIDER_HANDLE_RADIUS, handleColor);
        DrawCircleLines(handleX, trackY, GUIConstants::SLIDER_HANDLE_RADIUS, BLACK);

        // Draw label if provided
        if (!m_label.empty())
        {
            int fontSize = GUIConstants::SLIDER_LABEL_FONT_SIZE;
            int textWidth = MeasureText(m_label.c_str(), fontSize);

            // Adjust font size if text is too wide
            while (textWidth > m_bounds.width && fontSize > GUIConstants::MIN_FONT_SIZE)
            {
                fontSize--;
                textWidth = MeasureText(m_label.c_str(), fontSize);
            }

            DrawText(m_label.c_str(), m_bounds.x, m_bounds.y - GUIConstants::SLIDER_LABEL_OFFSET, fontSize, BLACK);
        }

        // Draw value text
        std::string valueText = std::to_string(static_cast<int>(m_currentValue));
        int valueFontSize = GUIConstants::SLIDER_VALUE_FONT_SIZE;
        int valueTextWidth = MeasureText(valueText.c_str(), valueFontSize);
        DrawText(valueText.c_str(), m_bounds.x + m_bounds.width - valueTextWidth, m_bounds.y - GUIConstants::SLIDER_LABEL_OFFSET, valueFontSize, BLACK);
    }

    void HandleInput(Vector2 offset) override
    {
        if (!m_enabled)
            return;

        Vector2 mousePos = GetMousePosition();
        float trackY = m_bounds.y + m_bounds.height / 2;
        Rectangle handleArea = {m_bounds.x, trackY - GUIConstants::SLIDER_HANDLE_RADIUS, m_bounds.width, GUIConstants::SLIDER_HANDLE_AREA_HEIGHT};

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePos, handleArea))
        {
            m_isDragging = true;
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            m_isDragging = false;
        }

        if (m_isDragging)
        {
            float normalizedPos = (mousePos.x - m_bounds.x) / m_bounds.width;
            normalizedPos = std::clamp(normalizedPos, 0.0f, 1.0f);

            float newValue = m_minValue + (normalizedPos * (m_maxValue - m_minValue));
            if (newValue != m_currentValue)
            {
                m_currentValue = newValue;
                if (m_onValueChanged)
                    m_onValueChanged(m_currentValue);
            }
        }
    }

    void SetOnValueChanged(std::function<void(float)> callback) { m_onValueChanged = callback; }
    float GetValue() const { return m_currentValue; }
    void SetValue(float value) { m_currentValue = std::clamp(value, m_minValue, m_maxValue); }

private:
    float m_minValue;
    float m_maxValue;
    float m_currentValue;
    std::string m_label;
    bool m_isDragging;
    std::function<void(float)> m_onValueChanged;
};

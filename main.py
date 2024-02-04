from fastapi import FastAPI, HTTPException, File, UploadFile
from pydantic import BaseModel
import serial
from google.cloud import vision
from openai import OpenAI
import asyncio
import cv2
import io

app = FastAPI()
vision_client = vision.ImageAnnotatorClient()
openAI_client = OpenAI(api_key="")
ser = serial.Serial('/dev/tty.usbmodem14101', 9600)

async def main():
    try:
        while True:
            if ser.in_waiting > 0:
                line = ser.readline().decode().strip()
                print("Received:", line)
                if line == "record":
                    await open_camera()
                    trash_category = await upload_to_vision("trash.jpg")
                    print(trash_category)
                    ser.write(trash_category.encode())
     
    finally:
        ser.close()



#@app.post("/upload/")
async def upload_to_vision(filepath):
    try:
        with io.open(filepath, 'rb') as image_file:
            content = image_file.read()
        image = vision.Image(content=content)
        response = vision_client.label_detection(image=image)
        labels = response.label_annotations

        detected_labels = []

        for label in labels:
            detected_labels.append(label.description)

        type_of_garbage = await classify_with_gpt(detected_labels)

        return type_of_garbage
    
    except Exception as e:
        print(e)
        raise HTTPException(status_code=500, detail="Error in upload")
    

async def classify_with_gpt(labels: list) -> str:
    try:
        input = f"Here is a description of an object: {labels}. With this list, classify to what category this object belongs to between these types of garbage: compost, trash, metal can, electronic, plastic. Do not invent a category, classify the object to one of the types of garbage I listed. If one of the words in the element of a list corresponds to a category, classify it as the object as this category. If there are body parts in the description, ignore. Your answer should simply be one word which is the category you classified this object to, all in lowercase."
        print(input)
        gpt_response = openAI_client.chat.completions.create(
            model="gpt-3.5-turbo",
            temperature=0.3,
            messages=[{"role": "system", "content": input}],
        )
        print("GPT Answer: ", gpt_response)
        return gpt_response.choices[0].message.content

    except Exception as e:
        print(e)
        raise HTTPException(status_code=500, detail="Error in gpt request")
    

async def open_camera():
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("Error: Couldn't open the camera")

    for _ in range(15):
        ret, frame = cap.read()
        if not ret:
            print("Error: Couldn't capture frame")
            break

    ret, frame = cap.read()

    if not ret:
        print("Error: Couldn't capture frame")

    cap.release()
    cv2.imwrite("trash.jpg", frame)

    cv2.destroyAllWindows()


if __name__ == "__main__":
    asyncio.run(main())

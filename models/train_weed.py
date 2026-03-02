from ultralytics import YOLO

def main():

    m = YOLO("yolov8s.pt")

    m.train(
        data="weed_dataset/data.yaml",
        epochs=65,
        imgsz=640,
        batch=16,
        patience=100,
        optimizer="auto",
        lr0=0.01,
        lrf=0.01,
        weight_decay=0.0005,
        warmup_epochs=3,
        device=0,
        workers=8,
        project="runs/detect",
        name="weed",
        pretrained=True,
        save=True,
        verbose=True
    )

if __name__ == "__main__":
    main()
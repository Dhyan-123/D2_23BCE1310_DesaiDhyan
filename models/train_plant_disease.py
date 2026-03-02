# ── Training hyperparameters ───────────────────────────────────────────────
TRAIN_ARGS = dict(
    # ---- Core ----
    task="detect",
    data=DATA_YAML,
    epochs=50,                  # K2 = 50 in pickle
    batch=32,                   # K  = 32 (0x20)
    imgsz=640,                  # M\x80\x02 = 640
    device=None,                # auto-select (CUDA when available)
    workers=8,

    # ---- Optimizer (auto-selected by Ultralytics) ----
    optimizer="auto",
    lr0=0.01,
    lrf=0.01,
    momentum=0.937,
    weight_decay=0.0005,

    # ---- Warmup ----
    warmup_epochs=3.0,
    warmup_momentum=0.8,
    warmup_bias_lr=0.0,

    # ---- Loss weights ----
    box=7.5,
    cls=0.5,
    dfl=1.5,

    # ---- LR schedule ----
    cos_lr=False,
    close_mosaic=10,            # disable mosaic last N epochs

    # ---- Regularization ----
    label_smoothing=0.0,
    dropout=0.0,

    # ---- AMP & precision ----
    amp=True,
    half=False,

    # ── Augmentation ──────────────────────────────────────────────────────
    # HSV colour jitter
    hsv_h=0.015,
    hsv_s=0.7,
    hsv_v=0.4,

    # Geometric
    degrees=0.0,
    translate=0.1,
    scale=0.5,
    shear=0.0,
    perspective=0.0,
    flipud=0.0,
    fliplr=0.5,
    bgr=0.0,

    # Mosaic / mixup (disabled — model was trained without them)
    mosaic=0.0,
    mixup=0.0,
    copy_paste=0.0,

    # Random erasing & crop
    auto_augment="randaugment",
    erasing=0.4,
    crop_fraction=1.0,

    # ── Misc ──────────────────────────────────────────────────────────────
    seed=0,
    deterministic=True,
    single_cls=False,
    rect=False,
    multi_scale=False,
    overlap_mask=True,
    mask_ratio=4,
    val=True,
    split="val",
    plots=True,
    save=True,
    save_period=-1,             # save only best & last
    val_period=1,
    patience=100,
    pretrained=True,
    exist_ok=True,
    resume=False,
    verbose=True,
    fraction=1.0,
    profile=False,
    freeze=None,
    cache=False,
    project=None,               # defaults to runs/detect
    name="train",               # output dir: runs/detect/train
    tracker="botsort.yaml",
)


def main():
    # Load pretrained YOLOv10-Small
    model = YOLOv10(MODEL_WEIGHTS)

    # Train
    results = model.train(**TRAIN_ARGS)

    print("\n── Training complete ──")
    print(f"Best weights : {results.save_dir}/weights/best.pt")
    print(f"Last weights : {results.save_dir}/weights/last.pt")

    # Quick validation on the best checkpoint
    metrics = model.val(
        data=DATA_YAML,
        imgsz=640,
        split="val",
        conf=0.25,
        iou=0.7,
        max_det=300,
    )
    print("\n── Validation metrics ──")
    print(metrics)

    return results, metrics


if __name__ == "__main__":
    main()
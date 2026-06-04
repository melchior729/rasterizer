(function () {
  const modelSelect = document.getElementById("model-select");
  const modeSelect = document.getElementById("mode-select");
  const lightSlider = document.getElementById("light-slider");
  const lightValue = document.getElementById("light-value");
  const overlayToggleBtn = document.getElementById("overlay-toggle-btn");
  const uploadBtn = document.getElementById("upload-btn");
  const uploadModal = document.getElementById("upload-modal");
  const uploadCancel = document.getElementById("upload-cancel");
  const uploadForm = document.getElementById("upload-form");
  const loadStatus = document.getElementById("load-status");

  const RAD_TO_DEG = 180 / Math.PI;
  const LIGHT_ANGLE_MAX = 2 * Math.PI;

  function wrapLightRad(rad) {
    if (!Number.isFinite(rad)) {
      return 0;
    }
    if (rad < 0) {
      rad = ((rad % LIGHT_ANGLE_MAX) + LIGHT_ANGLE_MAX) % LIGHT_ANGLE_MAX;
      return rad;
    }
    if (rad > LIGHT_ANGLE_MAX) {
      rad = rad % LIGHT_ANGLE_MAX;
      if (rad < 0) {
        rad += LIGHT_ANGLE_MAX;
      }
    }
    return rad;
  }

  function formatLightDeg(rad) {
    const wrapped = wrapLightRad(rad);
    if (wrapped <= 0.0001) {
      return 0;
    }
    if (Math.abs(wrapped - LIGHT_ANGLE_MAX) < 0.0001) {
      return 360;
    }
    return Math.round(wrapped * RAD_TO_DEG);
  }

  function setLightSlider(rad) {
    lightSlider.value = String(wrapLightRad(rad));
  }

  function setLoadStatus(text, hidden) {
    loadStatus.textContent = text;
    loadStatus.classList.toggle("hidden", hidden);
  }

  function wasmReady() {
    return typeof Module !== "undefined" && typeof Module._set_model === "function";
  }

  function callWasm(fn, ...args) {
    if (!wasmReady()) {
      return;
    }
    Module[fn](...args);
  }

  function syncUiToWasm() {
    callWasm("_set_model", Number(modelSelect.value));
    callWasm("_set_render_mode", Number(modeSelect.value));
    callWasm("_set_light_angle", wrapLightRad(Number(lightSlider.value)));
    updateLightLabel();
  }

  function updateLightLabel() {
    lightValue.textContent = `${formatLightDeg(Number(lightSlider.value))}°`;
  }

  function syncModelSelect(id) {
    document.getElementById("model-select").value = String(id);
  }

  function syncModeSelect(mode) {
    document.getElementById("mode-select").value = String(mode);
  }

  function syncLightAngle(rad) {
    setLightSlider(rad);
    updateLightLabel();
  }

  window.syncModelSelect = syncModelSelect;
  window.syncModeSelect = syncModeSelect;
  window.syncLightAngle = syncLightAngle;

  function openModal() {
    uploadModal.classList.remove("hidden");
  }

  function closeModal() {
    uploadModal.classList.add("hidden");
    uploadForm.reset();
  }

  modelSelect.addEventListener("change", () => {
    callWasm("_set_model", Number(modelSelect.value));
  });

  modeSelect.addEventListener("change", () => {
    callWasm("_set_render_mode", Number(modeSelect.value));
  });

  lightSlider.max = String(LIGHT_ANGLE_MAX);

  lightSlider.addEventListener("input", () => {
    const rad = wrapLightRad(Number(lightSlider.value));
    setLightSlider(rad);
    callWasm("_set_light_angle", rad);
    updateLightLabel();
  });

  overlayToggleBtn.addEventListener("click", () => {
    callWasm("_toggle_overlay");
  });

  uploadBtn.addEventListener("click", openModal);
  uploadCancel.addEventListener("click", closeModal);
  uploadModal.addEventListener("click", (e) => {
    if (e.target === uploadModal) {
      closeModal();
    }
  });

  uploadForm.addEventListener("submit", (e) => {
    e.preventDefault();
    closeModal();
    setLoadStatus(
      "Upload will be enabled soon — OBJ, optional MTL, and textures.",
      false
    );
    window.setTimeout(() => setLoadStatus("", true), 4000);
  });

  updateLightLabel();

  const prevOnRuntimeInitialized = window.Module?.onRuntimeInitialized;

  const canvas = document.getElementById("canvas");
  canvas.addEventListener("contextmenu", (e) => e.preventDefault());

  window.Module = {
    canvas,
    setStatus(text) {
      if (text) {
        setLoadStatus(text, false);
      } else {
        setLoadStatus("", true);
      }
    },
    onRuntimeInitialized() {
      if (prevOnRuntimeInitialized) {
        prevOnRuntimeInitialized();
      }
      syncUiToWasm();
      setLoadStatus("", true);
    },
  };
})();

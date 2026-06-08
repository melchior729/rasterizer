(function () {
  const modelSelect = document.getElementById("model-select");
  const modeSelect = document.getElementById("mode-select");
  const lightSlider = document.getElementById("light-slider");
  const lightValue = document.getElementById("light-value");
  const overlayToggleBtn = document.getElementById("overlay-toggle-btn");
  const zoomInBtn = document.getElementById("zoom-in-btn");
  const zoomOutBtn = document.getElementById("zoom-out-btn");
  const handBtn = document.getElementById("hand-btn");
  const uploadBtn = document.getElementById("upload-btn");
  const uploadModal = document.getElementById("upload-modal");
  const uploadClose = document.getElementById("upload-close");
  const uploadCancel = document.getElementById("upload-cancel");
  const uploadForm = document.getElementById("upload-form");
  const uploadChecklistToggle = document.getElementById("upload-checklist-toggle");
  const uploadChecklistPanel = document.getElementById("upload-checklist-panel");
  const settingsToggle = document.getElementById("settings-toggle");
  const settingsModal = document.getElementById("settings-modal");
  const settingsClose = document.getElementById("settings-close");
  const helpToggle = document.getElementById("help-toggle");
  const helpModal = document.getElementById("help-modal");
  const helpClose = document.getElementById("help-close");
  const loadStatus = document.getElementById("load-status");
  const canvas = document.getElementById("canvas");

  const RAD_TO_DEG = 180 / Math.PI;
  const LIGHT_ANGLE_MAX = 2 * Math.PI;

  const SENS_LEVEL_MIN = 1;
  const SENS_LEVEL_MAX = 20;
  const SENS_LEVEL_SPAN = SENS_LEVEL_MAX - SENS_LEVEL_MIN;

  const SENSITIVITY_SPECS = {
    rotate: { min: 0.001, max: 0.02, defaultLevel: 5, wasmFn: "_set_rotate_sens" },
    pan: { min: 0.01, max: 0.2, defaultLevel: 5, wasmFn: "_set_pan_sens" },
    zoom: { min: 0.05, max: 1.0, defaultLevel: 10, wasmFn: "_set_zoom_sens" },
  };

  const sensitivitySliders = [
    {
      key: "rotate",
      slider: document.getElementById("rotate-sens-slider"),
      label: document.getElementById("rotate-sens-level"),
    },
    {
      key: "pan",
      slider: document.getElementById("pan-sens-slider"),
      label: document.getElementById("pan-sens-level"),
    },
    {
      key: "zoom",
      slider: document.getElementById("zoom-sens-slider"),
      label: document.getElementById("zoom-sens-level"),
    },
  ];

  const sensitivityValues = {};

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

  function updateRangeFill(slider) {
    const min = Number(slider.min);
    const max = Number(slider.max);
    const val = Number(slider.value);
    const pct = max > min ? ((val - min) / (max - min)) * 100 : 0;
    slider.style.setProperty("--range-fill", `${pct}%`);
  }

  function levelToValue(level, spec) {
    const t = (level - SENS_LEVEL_MIN) / SENS_LEVEL_SPAN;
    return spec.min + t * (spec.max - spec.min);
  }

  function pushSensitivityToWasm(key) {
    const spec = SENSITIVITY_SPECS[key];
    callWasm(spec.wasmFn, sensitivityValues[key]);
  }

  function syncSensitivityToWasm() {
    for (const key of Object.keys(SENSITIVITY_SPECS)) {
      pushSensitivityToWasm(key);
    }
  }

  function syncSensitivitySlider({ key, slider, label }) {
    const level = Number(slider.value);
    const spec = SENSITIVITY_SPECS[key];
    sensitivityValues[key] = levelToValue(level, spec);
    label.textContent = `${level}/${SENS_LEVEL_MAX}`;
    updateRangeFill(slider);
    pushSensitivityToWasm(key);
  }

  function initSensitivitySliders() {
    for (const entry of sensitivitySliders) {
      const { key, slider } = entry;
      slider.value = String(SENSITIVITY_SPECS[key].defaultLevel);
      syncSensitivitySlider(entry);
      slider.addEventListener("input", () => {
        syncSensitivitySlider(entry);
      });
    }
  }

  function setLightSlider(rad) {
    lightSlider.value = String(wrapLightRad(rad));
    updateRangeFill(lightSlider);
  }

  function setLoadStatus(text, hidden) {
    loadStatus.textContent = text;
    loadStatus.classList.toggle("hidden", hidden);
    if (hidden) {
      loadStatus.classList.remove("load-status--error");
    }
  }

  function setLoadFailure(text) {
    loadStatus.textContent = text;
    loadStatus.classList.remove("hidden");
    loadStatus.classList.add("load-status--error");
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

  async function readFileAsUint8Array(file) {
    const buffer = await file.arrayBuffer();
    return new Uint8Array(buffer);
  }

  function writeToMemfs(path, data) {
    Module.FS.writeFile(path, data);
  }

  async function handleUploadSubmit() {
    if (!wasmReady() || !Module.FS) {
      setLoadFailure("Rasterizer is not ready.");
      return;
    }

    const objInput = document.getElementById("obj-file");
    const mtlInput = document.getElementById("mtl-file");
    const textureInput = document.getElementById("texture-files");
    const objFile = objInput.files?.[0];

    if (!objFile) {
      setLoadFailure("OBJ file is required.");
      return;
    }

    setLoadStatus("Loading model…", false);

    try {
      writeToMemfs("models/user.obj", await readFileAsUint8Array(objFile));

      const mtlFile = mtlInput.files?.[0];
      if (mtlFile) {
        writeToMemfs(`models/${mtlFile.name}`, await readFileAsUint8Array(mtlFile));
      }

      const textureFiles = textureInput.files;
      if (textureFiles?.length) {
        for (const texFile of textureFiles) {
          writeToMemfs(`textures/${texFile.name}`, await readFileAsUint8Array(texFile));
        }
      }

      Module._reload_user_mesh();

      if (Module._has_user_mesh()) {
        syncModelSelect(5);
        callWasm("_set_model", 5);
        closeModal();
        setLoadStatus("Model loaded.", false);
        window.setTimeout(() => setLoadStatus("", true), 3000);
      } else {
        setLoadFailure("Could not load model. Check your files and try again.");
      }
    } catch (err) {
      setLoadFailure(`Upload failed: ${err}`);
    }
  }

  function syncUiToWasm() {
    callWasm("_set_model", Number(modelSelect.value));
    callWasm("_set_render_mode", Number(modeSelect.value));
    callWasm("_set_light_angle", wrapLightRad(Number(lightSlider.value)));
    syncSensitivityToWasm();
    updateLightLabel();
  }

  function updateLightLabel() {
    lightValue.textContent = `${formatLightDeg(Number(lightSlider.value))}°`;
  }

  function enhanceSelect(selectEl) {
    const wrap = document.createElement("div");
    wrap.className = "ctrl-select";
    selectEl.parentNode.insertBefore(wrap, selectEl);
    wrap.append(selectEl);

    selectEl.classList.add("ctrl-select-native");
    selectEl.tabIndex = -1;
    selectEl.setAttribute("aria-hidden", "true");

    const trigger = document.createElement("button");
    trigger.type = "button";
    trigger.className = "ctrl-select-trigger";
    trigger.setAttribute("aria-haspopup", "listbox");
    trigger.setAttribute("aria-expanded", "false");
    trigger.id = `${selectEl.id}-trigger`;

    const fieldLabel = document.querySelector(`label[for="${selectEl.id}"]`);
    if (fieldLabel) {
      fieldLabel.setAttribute("for", trigger.id);
    }

    const label = document.createElement("span");
    trigger.append(label);

    const menu = document.createElement("ul");
    menu.className = "ctrl-select-menu";
    menu.setAttribute("role", "listbox");
    menu.id = `${selectEl.id}-listbox`;
    menu.hidden = true;
    trigger.setAttribute("aria-controls", menu.id);

    for (const opt of selectEl.options) {
      const item = document.createElement("li");
      item.className = "ctrl-select-option";
      item.setAttribute("role", "option");
      item.dataset.value = opt.value;
      item.textContent = opt.textContent;
      if (opt.selected) {
        item.setAttribute("aria-selected", "true");
      }
      menu.appendChild(item);
    }

    wrap.insertBefore(trigger, selectEl);
    wrap.appendChild(menu);

    function updateDisplay() {
      const selected = selectEl.options[selectEl.selectedIndex];
      label.textContent = selected ? selected.textContent : "";
      for (const item of menu.querySelectorAll(".ctrl-select-option")) {
        const isSelected = item.dataset.value === selectEl.value;
        item.setAttribute("aria-selected", isSelected ? "true" : "false");
      }
    }

    function positionMenu() {
      const rect = trigger.getBoundingClientRect();
      const gap = 4;
      const margin = 8;

      menu.style.width = `${rect.width}px`;
      menu.style.maxHeight = "";
      menu.classList.remove("ctrl-select-menu--scroll");

      const menuHeight = menu.scrollHeight;
      const spaceBelow = window.innerHeight - rect.bottom - gap - margin;
      const spaceAbove = rect.top - gap - margin;

      let top;
      if (menuHeight <= spaceBelow) {
        top = rect.bottom + gap;
      } else if (menuHeight <= spaceAbove) {
        top = rect.top - gap - menuHeight;
      } else if (spaceBelow >= spaceAbove) {
        top = rect.bottom + gap;
        menu.style.maxHeight = `${spaceBelow}px`;
        menu.classList.add("ctrl-select-menu--scroll");
      } else {
        top = Math.max(margin, rect.top - gap - spaceAbove);
        menu.style.maxHeight = `${spaceAbove}px`;
        menu.classList.add("ctrl-select-menu--scroll");
      }

      menu.style.top = `${top}px`;

      const maxLeft = window.innerWidth - rect.width - margin;
      menu.style.left = `${Math.min(Math.max(margin, rect.left), maxLeft)}px`;
    }

    function openMenu() {
      menu.hidden = false;
      trigger.setAttribute("aria-expanded", "true");
      document.body.appendChild(menu);
      positionMenu();
    }

    function closeMenu() {
      menu.hidden = true;
      trigger.setAttribute("aria-expanded", "false");
      wrap.appendChild(menu);
    }

    trigger.addEventListener("click", (e) => {
      e.stopPropagation();
      if (menu.hidden) {
        openMenu();
      } else {
        closeMenu();
      }
    });

    menu.addEventListener("click", (e) => {
      e.stopPropagation();
      const item = e.target.closest(".ctrl-select-option");
      if (!item) {
        return;
      }
      selectEl.value = item.dataset.value;
      selectEl.dispatchEvent(new Event("change", { bubbles: true }));
      updateDisplay();
      closeMenu();
    });

    document.addEventListener("click", (e) => {
      if (!wrap.contains(e.target) && !menu.contains(e.target)) {
        closeMenu();
      }
    });

    document.addEventListener("keydown", (e) => {
      if (e.key === "Escape") {
        closeMenu();
      }
    });

    window.addEventListener(
      "resize",
      () => {
        if (!menu.hidden) {
          positionMenu();
        }
      },
      { passive: true }
    );

    window.addEventListener(
      "scroll",
      () => {
        if (!menu.hidden) {
          positionMenu();
        }
      },
      { passive: true, capture: true }
    );

    updateDisplay();
    return { updateDisplay, closeMenu };
  }

  const modelCustom = enhanceSelect(modelSelect);
  const modeCustom = enhanceSelect(modeSelect);

  function syncModelSelect(id) {
    modelSelect.value = String(id);
    modelCustom.updateDisplay();
  }

  function syncModeSelect(mode) {
    modeSelect.value = String(mode);
    modeCustom.updateDisplay();
  }

  function syncLightAngle(rad) {
    setLightSlider(rad);
    updateLightLabel();
  }

  window.syncModelSelect = syncModelSelect;
  window.syncModeSelect = syncModeSelect;
  window.syncLightAngle = syncLightAngle;

  function setDropZoneDisplay(nameEl, textEl, files, emptyText) {
    if (!files?.length) {
      nameEl.textContent = "";
      nameEl.classList.add("hidden");
      textEl.textContent = emptyText;
      textEl.classList.remove("hidden");
      return;
    }

    nameEl.textContent = files.length === 1 ? files[0].name : `${files.length} files selected`;
    nameEl.classList.remove("hidden");
    textEl.classList.add("hidden");
  }

  function assignFilesToInput(input, files) {
    const dt = new DataTransfer();
    for (const file of files) {
      dt.items.add(file);
    }
    input.files = dt.files;
    input.dispatchEvent(new Event("change", { bubbles: true }));
  }

  function fileMatchesAccept(file, accept) {
    return accept.split(",").some((rule) => {
      const trimmed = rule.trim().toLowerCase();
      if (trimmed.startsWith(".")) {
        return file.name.toLowerCase().endsWith(trimmed);
      }
      if (trimmed.endsWith("/*")) {
        return file.type.startsWith(trimmed.slice(0, -1));
      }
      return file.type === trimmed;
    });
  }

  function initDropZone({ dropEl, input, nameEl, textEl, emptyText, multiple, accept }) {
    input.addEventListener("change", () => {
      setDropZoneDisplay(nameEl, textEl, input.files, emptyText);
    });

    dropEl.addEventListener("dragover", (e) => {
      e.preventDefault();
      dropEl.classList.add("is-dragover");
    });

    dropEl.addEventListener("dragleave", () => {
      dropEl.classList.remove("is-dragover");
    });

    dropEl.addEventListener("drop", (e) => {
      e.preventDefault();
      dropEl.classList.remove("is-dragover");
      const files = [...e.dataTransfer.files].filter((file) => fileMatchesAccept(file, accept));
      if (!files.length) {
        return;
      }
      assignFilesToInput(input, multiple ? files : [files[0]]);
    });
  }

  function resetUploadUi() {
    setDropZoneDisplay(
      document.getElementById("obj-name"),
      document.querySelector("#obj-drop .upload-drop-text"),
      null,
      "Drop .obj file or click to browse"
    );
    setDropZoneDisplay(
      document.getElementById("mtl-name"),
      document.querySelector("#mtl-drop .upload-drop-text"),
      null,
      "Drop .mtl file or click to browse"
    );
    setDropZoneDisplay(
      document.getElementById("texture-name"),
      document.querySelector("#texture-drop .upload-drop-text"),
      null,
      "Drop image files or click to browse"
    );
    uploadChecklistToggle.setAttribute("aria-expanded", "false");
    uploadChecklistPanel.classList.add("hidden");
    document.querySelectorAll(".upload-drop").forEach((el) => el.classList.remove("is-dragover"));
  }

  function closeModal() {
    uploadModal.classList.add("hidden");
    uploadForm.reset();
    resetUploadUi();
  }

  function openSettingsModal() {
    settingsModal.classList.remove("hidden");
  }

  function closeSettingsModal() {
    settingsModal.classList.add("hidden");
  }

  function openHelpModal() {
    helpModal.classList.remove("hidden");
  }

  function closeHelpModal() {
    helpModal.classList.add("hidden");
  }

  function openModal() {
    uploadModal.classList.remove("hidden");
  }

  modelSelect.addEventListener("change", () => {
    modelCustom.updateDisplay();
    callWasm("_set_model", Number(modelSelect.value));
  });

  modeSelect.addEventListener("change", () => {
    modeCustom.updateDisplay();
    callWasm("_set_render_mode", Number(modeSelect.value));
  });

  lightSlider.max = String(LIGHT_ANGLE_MAX);

  lightSlider.addEventListener("input", () => {
    const rad = wrapLightRad(Number(lightSlider.value));
    setLightSlider(rad);
    callWasm("_set_light_angle", rad);
    updateLightLabel();
  });

  function dispatchWheelZoom(zoomIn) {
    const wheelOpts = {
      deltaY: zoomIn ? -120 : 120,
      deltaMode: WheelEvent.DOM_DELTA_PIXEL,
      bubbles: true,
      cancelable: true,
    };
    canvas.dispatchEvent(new WheelEvent("wheel", wheelOpts));
  }

  zoomInBtn.addEventListener("click", () => {
    dispatchWheelZoom(true);
  });

  zoomOutBtn.addEventListener("click", () => {
    dispatchWheelZoom(false);
  });

  handBtn.addEventListener("click", () => {
    const active = !handBtn.classList.contains("is-active");
    handBtn.classList.toggle("is-active", active);
    handBtn.setAttribute("aria-pressed", active ? "true" : "false");
    handBtn.title = active
      ? "Pan with left drag (tap to use rotate)"
      : "Hand tool — tap to pan with left drag";
    callWasm("_toggle_swap_mouse");
  });

  overlayToggleBtn.addEventListener("click", () => {
    callWasm("_toggle_overlay");
  });

  uploadBtn.addEventListener("click", openModal);
  uploadClose.addEventListener("click", closeModal);
  uploadCancel.addEventListener("click", closeModal);
  uploadChecklistToggle.addEventListener("click", () => {
    const expanded = uploadChecklistToggle.getAttribute("aria-expanded") === "true";
    uploadChecklistToggle.setAttribute("aria-expanded", expanded ? "false" : "true");
    uploadChecklistPanel.classList.toggle("hidden", expanded);
  });

  initDropZone({
    dropEl: document.getElementById("obj-drop"),
    input: document.getElementById("obj-file"),
    nameEl: document.getElementById("obj-name"),
    textEl: document.querySelector("#obj-drop .upload-drop-text"),
    emptyText: "Drop .obj file or click to browse",
    multiple: false,
    accept: ".obj",
  });
  initDropZone({
    dropEl: document.getElementById("mtl-drop"),
    input: document.getElementById("mtl-file"),
    nameEl: document.getElementById("mtl-name"),
    textEl: document.querySelector("#mtl-drop .upload-drop-text"),
    emptyText: "Drop .mtl file or click to browse",
    multiple: false,
    accept: ".mtl",
  });
  initDropZone({
    dropEl: document.getElementById("texture-drop"),
    input: document.getElementById("texture-files"),
    nameEl: document.getElementById("texture-name"),
    textEl: document.querySelector("#texture-drop .upload-drop-text"),
    emptyText: "Drop image files or click to browse",
    multiple: true,
    accept: "image/*",
  });
  uploadModal.addEventListener("click", (e) => {
    if (e.target === uploadModal) {
      closeModal();
    }
  });

  settingsToggle.addEventListener("click", openSettingsModal);
  settingsClose.addEventListener("click", closeSettingsModal);
  settingsModal.addEventListener("click", (e) => {
    if (e.target === settingsModal) {
      closeSettingsModal();
    }
  });

  helpToggle.addEventListener("click", openHelpModal);
  helpClose.addEventListener("click", closeHelpModal);
  helpModal.addEventListener("click", (e) => {
    if (e.target === helpModal) {
      closeHelpModal();
    }
  });

  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && !uploadModal.classList.contains("hidden")) {
      closeModal();
    }
    if (e.key === "Escape" && !settingsModal.classList.contains("hidden")) {
      closeSettingsModal();
    }
    if (e.key === "Escape" && !helpModal.classList.contains("hidden")) {
      closeHelpModal();
    }
  });

  uploadForm.addEventListener("submit", (e) => {
    e.preventDefault();
    handleUploadSubmit();
  });

  updateLightLabel();
  updateRangeFill(lightSlider);
  initSensitivitySliders();

  window.sensitivityValues = sensitivityValues;

  const prevOnRuntimeInitialized = window.Module?.onRuntimeInitialized;
  const prevOnAbort = window.Module?.onAbort;

  canvas.addEventListener("contextmenu", (e) => e.preventDefault());

  window.Module = window.Module || {};
  Object.assign(window.Module, {
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
    onAbort(reason) {
      if (prevOnAbort) {
        prevOnAbort(reason);
      }
      setLoadFailure(`Rasterizer failed: ${reason}`);
    },
    printErr(text) {
      console.error(text);
    },
  });
})();

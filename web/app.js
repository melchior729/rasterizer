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
  const uploadCancel = document.getElementById("upload-cancel");
  const uploadForm = document.getElementById("upload-form");
  const helpToggle = document.getElementById("help-toggle");
  const helpModal = document.getElementById("help-modal");
  const helpClose = document.getElementById("help-close");
  const loadStatus = document.getElementById("load-status");
  const canvas = document.getElementById("canvas");

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

  function updateRangeFill(slider) {
    const min = Number(slider.min);
    const max = Number(slider.max);
    const val = Number(slider.value);
    const pct = max > min ? ((val - min) / (max - min)) * 100 : 0;
    slider.style.setProperty("--range-fill", `${pct}%`);
  }

  function setLightSlider(rad) {
    lightSlider.value = String(wrapLightRad(rad));
    updateRangeFill(lightSlider);
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

  function closeModal() {
    uploadModal.classList.add("hidden");
    uploadForm.reset();
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
  uploadCancel.addEventListener("click", closeModal);
  uploadModal.addEventListener("click", (e) => {
    if (e.target === uploadModal) {
      closeModal();
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
    if (e.key === "Escape" && !helpModal.classList.contains("hidden")) {
      closeHelpModal();
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
  updateRangeFill(lightSlider);

  const prevOnRuntimeInitialized = window.Module?.onRuntimeInitialized;

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

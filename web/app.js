(function () {
  const THEME_KEY = "theme";
  const themeToggle = document.getElementById("theme-toggle");
  const modelSelect = document.getElementById("model-select");
  const modeSelect = document.getElementById("mode-select");
  const lightSlider = document.getElementById("light-slider");
  const lightValue = document.getElementById("light-value");
  const uploadBtn = document.getElementById("upload-btn");
  const uploadModal = document.getElementById("upload-modal");
  const uploadCancel = document.getElementById("upload-cancel");
  const uploadForm = document.getElementById("upload-form");
  const loadStatus = document.getElementById("load-status");
  const modeBlurb = document.getElementById("mode-blurb");

  const modeBlurbs = {
    0: "Wireframe draws triangle edges only — useful for inspecting mesh topology.",
    1: "Flat shading uses one normal per face, giving a faceted look.",
    2: "Gouraud shading interpolates vertex lighting across each triangle.",
    3: "Phong shading interpolates normals and computes specular highlights per pixel.",
  };

  function getTheme() {
    return document.documentElement.getAttribute("data-theme") === "sun"
      ? "sun"
      : "moon";
  }

  let pendingDayMode = null;

  function setTheme(theme) {
    const isSun = theme === "sun";
    if (isSun) {
      document.documentElement.setAttribute("data-theme", "sun");
    } else {
      document.documentElement.removeAttribute("data-theme");
    }
    localStorage.setItem(THEME_KEY, isSun ? "sun" : "moon");
    const label = isSun ? "Switch to dark mode" : "Switch to light mode";
    themeToggle.setAttribute("aria-label", label);
    themeToggle.title = label;
    syncThemeToWasm();
  }

  function syncThemeToWasm() {
    const day = getTheme() === "sun" ? 1 : 0;
    if (typeof Module !== "undefined" && typeof Module._set_day_mode === "function") {
      Module._set_day_mode(day);
      pendingDayMode = null;
      return;
    }
    pendingDayMode = day;
  }

  themeToggle.addEventListener("click", () => {
    setTheme(getTheme() === "moon" ? "sun" : "moon");
  });

  setTheme(localStorage.getItem(THEME_KEY) === "sun" ? "sun" : "moon");

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
    callWasm("_set_light_angle", Number(lightSlider.value));
    syncThemeToWasm();
    updateLightLabel();
    updateModeBlurb();
  }

  function updateLightLabel() {
    const v = Number(lightSlider.value);
    lightValue.textContent = `${v.toFixed(2)} rad`;
  }

  function updateModeBlurb() {
    modeBlurb.textContent = modeBlurbs[modeSelect.value] ?? "";
  }

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
    updateModeBlurb();
  });

  lightSlider.addEventListener("input", () => {
    callWasm("_set_light_angle", Number(lightSlider.value));
    updateLightLabel();
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
  updateModeBlurb();

  const prevOnRuntimeInitialized = window.Module?.onRuntimeInitialized;

  window.Module = {
    canvas: document.getElementById("canvas"),
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
      if (pendingDayMode !== null && typeof Module._set_day_mode === "function") {
        Module._set_day_mode(pendingDayMode);
        pendingDayMode = null;
      }
      setLoadStatus("", true);
    },
  };
})();

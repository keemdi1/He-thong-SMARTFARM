// Khởi tạo các phần tử DOM
let btn1 = document.querySelector('#btn1');
let btn2 = document.querySelector('#btn2');
let imgpum = document.querySelector('#pum');

let btn3 = document.querySelector('#btn3');
let btn4 = document.querySelector('#btn4');
let imgmist = document.querySelector('#mist');

let btn5 = document.querySelector('#btn5');
let btn6 = document.querySelector('#btn6');
let imgvalve = document.querySelector('#valve');

let btn7 = document.querySelector('#btn7');
let btn8 = document.querySelector('#btn8');
let imglight = document.querySelector('#light');

let btn9 = document.querySelector('#btn9');
let btn10 = document.querySelector('#btn10');
let imgfan = document.querySelector('#fan')

let btn11 = document.querySelector('#btn11');
let btn12 = document.querySelector('#btn12');
let imgvalve2 = document.querySelector('#valve2');
// NÚT AUTO-------------------------------
const btn = document.getElementById("autoBtn");

btn.addEventListener("click", () => {
  btn.classList.toggle("on");
  if (btn.classList.contains("on")) {
    btn.textContent = "ON";
    firebase.database().ref("autoBtn").set(1);
  } else {
      btn.textContent = "OFF";
      firebase.database().ref("autoBtn").set(0);
    }
  });

// NÚT "?" thuộc tính ----------------------
const attrBtn = document.getElementById("attrBtn");
const infoSpan = document.getElementById("info");
let infoVisible = false;

// Toggle hiển thị khi nhấn nút "?"
attrBtn.addEventListener("click", (e) => {
    e.stopPropagation(); // Ngăn sự kiện lan ra ngoài
    infoVisible = !infoVisible;
    infoSpan.style.display = infoVisible ? "block" : "none";
});

// Ngăn click vào bảng làm mất bảng
infoSpan.addEventListener("click", (e) => {
    e.stopPropagation();
});

  // Click bất kỳ nơi nào ngoài bảng thì ẩn
document.addEventListener("click", () => {
    if (infoVisible) {
      infoSpan.style.display = "none";
      infoVisible = false;
    }
}); 

// ======== HIỂN THỊ POPUP CẢNH BÁO AUTO ====================
const autoPopup = document.getElementById('autoPopup');

firebase.database().ref("autoBtn").on("value", (snapshot) => {
  const value = snapshot.val();
  if (value === 1) {
    autoPopup.classList.remove("hidden"); // Hiện popup
  } else {
    autoPopup.classList.add("hidden"); // Ẩn popup
  }
});

////////////////////////////////////////////////////////////////////12 bơm
btn1.addEventListener('click', () => {
  imgpum.src = 'img/pumpon1.png'; // ảnh khi bật
  btn1.classList.add('active-on');
  btn2.classList.remove('active-off');
  btn2.classList.remove('active-on'); // Nếu có nhấn nhầm
  btn1.classList.remove('active-off'); // Đảm bảo không có class sai
  firebase.database().ref("thietbi1").set({ bom: 1 });
});

btn2.addEventListener('click', () => {
  imgpum.src = "img/pump.png"; // ảnh khi tắt
  btn2.classList.add('active-off');
  btn1.classList.remove('active-on');
  btn1.classList.remove('active-off');
  btn2.classList.remove('active-on');
  firebase.database().ref("thietbi1").set({ bom: 0 });
});
  ////////////////////////////////////////////////////////////////////34 phun sương
btn3.addEventListener('click', () => {
    imgmist.src = 'img/miston.png'; // ảnh khi bật
    btn3.classList.add('active-on');
    btn4.classList.remove('active-off');
    btn4.classList.remove('active-on');
    btn3.classList.remove('active-off');
    firebase.database().ref("thietbi2").set({ phun_suong: 1 });
  });
  
  btn4.addEventListener('click', () => {
    imgmist.src = 'img/mist.png'; // ảnh khi tắt
    btn4.classList.add('active-off');
    btn3.classList.remove('active-on');
    btn3.classList.remove('active-off');
    btn4.classList.remove('active-on');
    firebase.database().ref("thietbi2").set({ phun_suong: 0 });
  });
  
  ////////////////////////////////////////////////////////////////////56 van xả
btn5.addEventListener('click', () => {
    imgvalve.src = 'img/valveon.png'; // ảnh khi bật
    btn5.classList.add('active-on');
    btn6.classList.remove('active-off');
    btn6.classList.remove('active-on');
    btn5.classList.remove('active-off');
    firebase.database().ref("thietbi3").set({ van_xa: 1 });
  });
  
  btn6.addEventListener('click', () => {
    imgvalve.src = 'img/valve.png'; // ảnh khi tắt
    btn6.classList.add('active-off');
    btn5.classList.remove('active-on');
    btn5.classList.remove('active-off');
    btn6.classList.remove('active-on');
    firebase.database().ref("thietbi3").set({ van_xa: 0 });
  });
  
////////////////////////////////////////////////////////////////////78 Đèn
btn7.addEventListener('click', () => {
  imglight.src = 'img/denon.png'; // ảnh khi bật
  btn7.classList.add('active-on');
  btn8.classList.remove('active-off');
  btn8.classList.remove('active-on');
  btn7.classList.remove('active-off');
  firebase.database().ref("thietbi4").set({ den: 1 });
});

btn8.addEventListener('click', () => {
  imglight.src = 'img/den.png'; // ảnh khi tắt
  btn8.classList.add('active-off');
  btn7.classList.remove('active-on');
  btn7.classList.remove('active-off');
  btn8.classList.remove('active-on');
  firebase.database().ref("thietbi4").set({ den: 0 });
});

////////////////////////////////////////////////////////////////////9 10 quạt
btn9.addEventListener('click', () => {
  imgfan.src = 'img/fanon.png'; // ảnh khi bật
  btn9.classList.add('active-on');
  btn10.classList.remove('active-off');
  btn10.classList.remove('active-on');
  btn9.classList.remove('active-off');
  firebase.database().ref("thietbi5").set({ quat: 1 });
});

btn10.addEventListener('click', () => {
  imgfan.src = 'img/fan.png'; // ảnh khi tắt
  btn10.classList.add('active-off');
  btn9.classList.remove('active-on');
  btn9.classList.remove('active-off');
  btn10.classList.remove('active-on');
  firebase.database().ref("thietbi5").set({ quat: 0 });
});
////////////////////////////////////////////////////////////////////11 12 valve2 
btn11.addEventListener('click', () => {
  imgvalve2.src = 'img/valve2on.png'; // ảnh khi bật
  btn11.classList.add('active-on');
  btn12.classList.remove('active-off');
  btn12.classList.remove('active-on');
  btn11.classList.remove('active-off');
  firebase.database().ref("thietbi6").set({valve_watering: 1 });
});

btn12.addEventListener('click', () => {
  imgvalve2.src = 'img/valve2.png'; // ảnh khi tắt
  btn12.classList.add('active-off');
  btn11.classList.remove('active-on');
  btn11.classList.remove('active-off');
  btn12.classList.remove('active-on');
  firebase.database().ref("thietbi6").set({ valve_watering: 0 });
});

// chọn cây trồng
function openPopup() {
      document.getElementById("plantPopup").style.display = "block";
    }

    function closePopup() {
      document.getElementById("plantPopup").style.display = "none";
    }

    const plantData = {
      "Cây cải thìa": {
        image: "img/thia.png",
        time: "28 - 35 ngày",
        description: "Cải thìa là loại rau giàu dinh dưỡng, giàu vitamin A, C, K và chất xơ, tốt cho miễn dịch và tiêu hóa...",
        conditions: `
          <ul>
            <li><strong>Nhiệt độ lý tưởng:</strong> 18°C – 24°C (Chịu được 13°C đến 28°C; nhiệt độ cao dễ gây hiện tượng ra hoa sớm)</li>
            <li><strong>Độ ẩm đất:</strong> 60% – 75% </li>
            <li><strong>Độ ẩm không khí:</strong> 50% – 70%</li>
            <li><strong>Chiếu sáng:</strong> 6 – 8 giờ/ngày (ánh sáng tán xạ hoặc nắng nhẹ)</li>
          </ul>
        `
      },

      "Cây xà lách": {
        image: "img/xalach.png",
        time: "35 - 40 ngày",
        description: "Xà lách là loại rau ăn sống phổ biến, giúp giảm viêm nhiễm, ngừa bệnh mãn tính, bảo vệ mắt, đặc biệt là võng mạc,...",
        conditions: `
          <ul>
            <li><strong>Nhiệt độ lý tưởng:</strong> 18°C – 24°C (Chịu được 13°C đến 28°C)</li>
            <li><strong>Độ ẩm đất:</strong> 65% – 75%</li>
            <li><strong>Độ ẩm không khí:</strong> 50% – 70%</li>
            <li><strong>Chiếu sáng:</strong> 4 – 6 giờ/ngày (Ưa nắng nhẹ)</li>
          </ul>
        `
      },

      "Cải bẹ xanh": {
      image: "img/bexanh.png",
      time: "30 - 45 ngày",
      description: "Cải bẹ xanh giúp thanh nhiệt, mát gan, giảm nóng trong cơ thể, tốt cho hệ tiêu hóa, giàu vitamin...",
      conditions: `
        <ul>
          <li><strong>Nhiệt độ lý tưởng:</strong> 22°C – 28°C (chấp nhận 20°C – 32°C)</li>
          <li><strong>Độ ẩm đất:</strong> 65% – 80%</li>
          <li><strong>Độ ẩm không khí:</strong> 70% – 85%</li>
          <li><strong>Chiếu sáng:</strong> 4 – 6 giờ/ngày (ánh sáng tán xạ hoặc nắng nhẹ)</li>
        </ul>
      `
      },

      "Cây cải ngọt": {
        image: "img/ngot.png",
        time: "25 – 40 ngày",
        description: "Cải ngọt là loại rau tốt cho người bị gout, bảo vệ gan, giàu canxi, ngăn ngừa loãng xương, phòng ngừa ung thư...",
        conditions: `
          <ul>
            <li><strong>Nhiệt độ lý tưởng:</strong> 20°C – 28°C (chịu được 18°C – 32°C)</li>
            <li><strong>Độ ẩm đất:</strong> 60% – 75%</li>
            <li><strong>Độ ẩm không khí:</strong> 70% – 80%</li>
            <li><strong>Chiếu sáng:</strong> 4 – 6 giờ/ngày (ánh sáng tán xạ hoặc nắng nhẹ)</li>
          </ul>
        `
      }
    };

function selectPlantByName(name){
    const plant = plantData[name];
    if(!plant) return;

    document.getElementById("mainImage").src = plant.image;
    document.getElementById("plant-name").innerText = name;
    document.getElementById("harvest-time").innerText = plant.time;
    document.getElementById("plant-description").innerText = plant.description;
    document.getElementById("plant-conditions").innerHTML = plant.conditions || "";

    localStorage.setItem("selected-plant-name", name);
    localStorage.setItem("selected-plant-image", plant.image);
    localStorage.setItem("selected-plant-time", plant.time);
    localStorage.setItem("selected-plant-description", plant.description);
    localStorage.setItem("selected-plant-conditions", plant.conditions || "");

    // **đóng popup ngay khi chọn cây**
    closePopup();
}

// Đóng popup khi click ra ngoài
window.addEventListener('click', function(event){
    const popup = document.getElementById("plantPopup");
    const content = document.querySelector(".pop-content");
    const mainImage = document.getElementById("mainImage");

    if(popup.style.display === "block" && !content.contains(event.target) && event.target !== mainImage){
        closePopup();
    }
});


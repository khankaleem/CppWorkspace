#include <iostream>
#include <memory>

class VendingMachine;

enum class StateIdentifier : uint8_t {
  NO_COIN = 0,
  HAS_COIN = 1,
  DISPENSING = 2,
  NUM = 3
};
constexpr int getNumStates() {
  return static_cast<int>(StateIdentifier::NUM);
}

class VendingMachineState {
public:
  VendingMachineState() = default;
  virtual ~VendingMachineState() = default;
  virtual void insert_coin(VendingMachine* machine) = 0;
  virtual void select_product(VendingMachine* machine) = 0;
  virtual void dispense(VendingMachine* machine) = 0;
};

class NoCoinState : public VendingMachineState {
public:
  NoCoinState() = default;
  void insert_coin(VendingMachine* machine) override;
  void select_product(VendingMachine* machine) override;
  void dispense(VendingMachine* machine) override;
};

class HasCoinState : public VendingMachineState {
public:
  HasCoinState() = default;
  void insert_coin(VendingMachine* machine) override;
  void select_product(VendingMachine* machine) override;
  void dispense(VendingMachine* machine) override;
};

class DispenseState : public VendingMachineState {
public:
  DispenseState() = default;
  void insert_coin(VendingMachine* machine) override;
  void select_product(VendingMachine* machine) override;
  void dispense(VendingMachine* machine) override;
};

class VendingMachine {
  std::array<std::unique_ptr<VendingMachineState>, getNumStates()> m_state_universe;
  VendingMachineState* m_state;
public:
  VendingMachine();

  void set_state(StateIdentifier new_state_) {
    m_state = m_state_universe[static_cast<int>(new_state_)].get();
  }
  void insert_coin() { 
    m_state->insert_coin(this); 
  }
  void select_product() { 
    m_state->select_product(this); 
  }
  void dispense() { 
    m_state->dispense(this); 
  }
};

VendingMachine::VendingMachine() : m_state_universe{
                                    std::make_unique<NoCoinState>(),
                                    std::make_unique<HasCoinState>(),
                                    std::make_unique<DispenseState>()} {
  if ( std::any_of(m_state_universe.begin(), m_state_universe.end(), [](const auto& ptr){ return ptr == nullptr; }) ) {
    throw std::logic_error{"states are missing\n"};
  }
  m_state = m_state_universe[static_cast<int>(StateIdentifier::NO_COIN)].get();
}

// no coin state logic
void NoCoinState::insert_coin(VendingMachine* machine) {
  std::cout << "Coin inserted!\n";
  machine->set_state(StateIdentifier::HAS_COIN);
}
void NoCoinState::select_product(VendingMachine* machine) { 
  std::cout << "Insert Coin First\n"; 
}
void NoCoinState::dispense(VendingMachine* machine) { 
  std::cout << "Insert Coin First\n"; 
}

// has coin state logic
void HasCoinState::insert_coin(VendingMachine* machine) { 
  std::cout << "Select Product First\n";
}
void HasCoinState::select_product(VendingMachine* machine) {
  std::cout << "Product Select!\n";
  machine->set_state(StateIdentifier::DISPENSING);
}
void HasCoinState::dispense(VendingMachine* machine) { 
  std::cout << "Select Product First\n"; 
}

// DispenseState Logic
void DispenseState::insert_coin(VendingMachine* machine) { 
  std::cout << "Please wait, Dispensing\n"; 
}
void DispenseState::select_product(VendingMachine* machine) { 
  std::cout << "Please wait, Dispensing\n"; 
}
void DispenseState::dispense(VendingMachine* machine) {
  std::cout << "Please Collect Product!\n";
  machine->set_state(StateIdentifier::NO_COIN);
}

// 6. MAIN
int main() {
  VendingMachine machine;
  machine.insert_coin();
  machine.select_product();
  machine.dispense();
  return 0;
}
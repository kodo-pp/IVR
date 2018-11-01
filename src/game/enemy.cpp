#include <modbox/core/core.hpp>
#include <modbox/game/enemy.hpp>
#include <modbox/game/game_loop.hpp>
#include <modbox/geometry/game_position.hpp>
#include <modbox/geometry/geometry.hpp>
#include <modbox/graphics/graphics.hpp>
#include <modbox/modules/module_io.hpp>
#include <modbox/util/util.hpp>

#include <boost/lexical_cast.hpp>
#include <cmath>

Enemy::Enemy(irr::scene::ISceneNode* _node, const std::string& _kind, EnemyId _id)
        : node(_node), kind(_kind), id(_id)
{
}

void Enemy::hit(double damage)
{
    healthLeft -= damage;
}

double Enemy::getHealthLeft() const
{
    return healthLeft;
}

void Enemy::setHealthLeft(double health)
{
    healthLeft = health;
}

double Enemy::getHealthMax() const
{
    return healthMax;
}
void Enemy::setHealthMax(double health)
{
    healthMax = health;
}

GamePosition Enemy::getPosition() const
{
    auto position = GamePosition(node->getPosition());
    return position;
}
void Enemy::setPosition(const GamePosition& newPosition)
{
    node->setPosition(newPosition.toIrrVector3df());
}

bool Enemy::isDead() const {
    return healthLeft <= 0;
}

irr::scene::ISceneNode* Enemy::sceneNode() const
{
    return node;
}

void Enemy::ai()
{
    static int counter = 0;
    double fps = /* getFps(); */ 60.0;
    double tickSpeed = /* getTickSpeed(); */ 10.0;
    int updateEach = static_cast<int>(round(fps / tickSpeed));
    if (counter >= updateEach) {
        counter = 0;
        std::string action = enemyManager.getAiFunction(kind)(id);
        std::vector<std::string> parts;
        boost::algorithm::split(parts, action, [](char c) { return c == ' '; });
        if (parts.size() == 0) {
            return;
        }
        if (parts.at(0) == "jump") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'jump' action");
            }
            float jumpSpeed = boost::lexical_cast<float>(parts.at(1));
            graphicsJump(node, jumpSpeed);
        } else if (parts.at(0) == "lookAt") {
            if (parts.size() != 4) {
                throw std::runtime_error("Invalid number of arguments for 'lookAt' action");
            }
            float x = boost::lexical_cast<float>(parts.at(1));
            float y = boost::lexical_cast<float>(parts.at(2));
            float z = boost::lexical_cast<float>(parts.at(3));
            graphicsLookAt(node, x, y, z);
        } else if (parts.at(0) == "setSpeed") {
            if (parts.size() != 2) {
                throw std::runtime_error("Invalid number of arguments for 'setSpeed' action");
            }
            movementSpeed = boost::lexical_cast<double>(parts.at(1));
        } else if (parts.at(0) == "attackPlayer") {
            if (parts.size() != 3) {
                throw std::runtime_error("Invalid number of arguments for 'attackPlayer' action");
            }
            auto maxDistance = boost::lexical_cast<double>(parts.at(1));
            auto damage = boost::lexical_cast<double>(parts.at(2));
            if (
                (
                    getPosition().toIrrVector3df()
                    - getPlayer().getPosition().toIrrVector3df()
                ).getLengthSQ() <= maxDistance * maxDistance
            ) {
                getPlayer().hit(damage);
            }
        } else {
            throw std::runtime_error(std::string("Unknown action: '") + parts.at(0) + "'");
        }
    } else {
        ++counter;
    }
}

EnemyId EnemyManager::createEnemy(const std::string& kind, irr::scene::ISceneNode* model)
{
    ++idCounter;
    enemies.emplace(idCounter, Enemy(model, kind, idCounter));
    creationFunctionsByKind.at(kind)(idCounter);
    return idCounter;
}

void EnemyManager::deleteEnemy(EnemyId id)
{
    enemies.erase(id);
}

const Enemy& EnemyManager::accessEnemy(EnemyId id)
{
    return enemies.at(id);
}
Enemy& EnemyManager::mutableAccessEnemy(EnemyId id)
{
    return enemies.at(id);
}

EnemyManager enemyManager;

// TODO: переместить в другое место
FuncResult handlerEachTickWithHandle(const std::vector<std::string>& args)
{
    if (args.size() != 2) {
        throw std::logic_error("Invalid number of arguments for handlerEnemySyncDrawable");
    }
    FuncResult ret;
    std::string name = getArgument<std::string>(args, 0);
    uint64_t param = getArgument<uint64_t>(args, 1);
    eachTickWithParam(name, param);

    return ret;
}

FuncResult handlerEnemyProcessAi(const std::vector<std::string>& args)
{
    if (args.size() != 1) {
        throw std::logic_error("Invalid number of arguments for handlerEnemyProcessAi");
    }
    FuncResult ret;

    auto instanceHandle = getArgument<uint64_t>(args, 0);
    auto& instance = getModuleClassInstance(instanceHandle);
    const auto& moduleClass = getModuleClass(instance.className);
    const auto& funcProvider = getFuncProvider(moduleClass.methods.at("ai").name);
    FuncResult result = funcProvider(args);
    auto action = getArgument<std::string>(result.data, 0);

    std::vector<std::string> split;
    boost::algorithm::split(split, action, [](char c) { return c == ';'; });

    auto modelHandle = instance.members.at("model").get<uint64_t>();
    for (const std::string& act : split) {
    }
    float x, y, z;
    graphicsGetPosition(drawablesManager.access(modelHandle), x, y, z);
    instance.members.at("x").set<double>(x);
    instance.members.at("y").set<double>(y);
    instance.members.at("z").set<double>(z);

    return ret;
}

void initializeEnemies()
{
    registerFuncProvider(
            FuncProvider("core.eachTickWithHandle", handlerEachTickWithHandle), "si", "");
    registerFuncProvider(FuncProvider("enemy.processAi", handlerEnemyProcessAi), "u", "");
    addModuleClass("graphics.Drawable",
                   ModuleClass({{"model", {'u'}}}, {}, "graphics.Drawable", ""));
    addModuleClass("game.Enemy",
                   ModuleClass({{"x", {'f'}}, {"y", {'f'}}, {"z", {'f'}}, {"hp", {'f'}}},
                               {{"ai", {"core.class.nop", "ub", "bs"}}},
                               "game.Enemy",
                               "graphics.Drawable"));
}

std::function <std::string(EnemyId)> EnemyManager::getAiFunction(const std::string& kind)
{
    std::lock_guard <std::recursive_mutex> lock(mutex);
    return aiFunctionsByKind.at(kind);
}
void EnemyManager::addKind(
    const std::string& kind,
    const std::function <void(EnemyId)>& creationFunction,
    const std::function <std::string(EnemyId)>& aiFunction
) {
    std::lock_guard <std::recursive_mutex> lock(mutex);
    if (aiFunctionsByKind.count(kind) > 0) {
        throw std::runtime_error("Enemy kind '" + kind + "' already registered");
    }
    aiFunctionsByKind.emplace(kind, aiFunction);
    creationFunctionsByKind.emplace(kind, creationFunction);
}
